extern "C" {
#include "php.h"
#include "Zend/zend_errors.h"
}
#include "ByteBufferReader.h"
#include "ByteBufferWriter.h"
#include "DataDecodeException.h"
#include "../Serializers.h"
#include <vector>

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_integer, 0, 1, IS_LONG, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferReader, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_integer, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferWriter, 0)
ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_float, 0, 1, IS_DOUBLE, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferReader, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_float, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferWriter, 0)
ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_array, 0, 2, IS_ARRAY, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferReader, 0)
ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_array, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferWriter, 0)
ZEND_ARG_ARRAY_INFO(0, values, 0)
ZEND_END_ARG_INFO()

template<typename TValue>
static inline void zval_long_wrapper(zval* zv, TValue value) {
	ZVAL_LONG(zv, value);
}

template<typename TValue>
static inline void zval_double_wrapper(zval* zv, TValue value) {
	ZVAL_DOUBLE(zv, value);
}

template<typename TValue>
static inline void return_long_array(zval* zv, std::vector<TValue>& valueArray) {
	array_init_size(zv, valueArray.size());

	for (size_t i = 0; i < valueArray.size(); i++) {
		add_next_index_long(zv, static_cast<zend_long>(valueArray[i]));
	}
}

template<typename TValue>
static inline void return_double_array(zval* zv, std::vector<TValue>& valueArray) {
	array_init_size(zv, valueArray.size());

	for (size_t i = 0; i < valueArray.size(); i++) {
		add_next_index_double(zv, static_cast<double>(valueArray[i]));
	}
}

template<typename TValue>
using readTypeFunc_t = bool (*)(unsigned char* bytes, size_t used, size_t& offset, TValue& result);

template<typename TValue>
using returnResultFunc_t = void(*)(zval*, TValue);

template<typename TValue, readTypeFunc_t<TValue> readTypeFunc, returnResultFunc_t<TValue> assignResult>
void ZEND_FASTCALL zif_readType(INTERNAL_FUNCTION_PARAMETERS) {
	zval* object_zv;
	byte_buffer_reader_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_reader_ce, 0, 0)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	object = READER_FROM_ZVAL(object_zv);

	TValue result;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(object->buffer));
	if (readTypeFunc(bytes, ZSTR_LEN(object->buffer), object->offset, result)) {
		assignResult(return_value, result);
	}
}

template<typename TValue>
using readTypeArrayFunc_t = bool (*) (unsigned char* bytes, size_t used, size_t& offset, size_t count, std::vector<TValue>& resultArray);

template<typename TValue, readTypeArrayFunc_t<TValue> readTypeArray, returnResultFunc_t<std::vector<TValue>&> returnResult>
void ZEND_FASTCALL zif_readTypeArray(INTERNAL_FUNCTION_PARAMETERS) {
	zval* object_zv;
	zend_long zcount;
	byte_buffer_reader_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_reader_ce, 0, 0)
		Z_PARAM_LONG(zcount)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	if (zcount < 1) {
		zend_value_error("Count must be at least 1");
		return;
	}
	size_t count = static_cast<size_t>(zcount);

	object = READER_FROM_ZVAL(object_zv);

	std::vector<TValue> resultArray;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(object->buffer));
	if (readTypeArray(bytes, ZSTR_LEN(object->buffer), object->offset, count, resultArray)) {
		returnResult(return_value, resultArray);
	}
}

template<typename TValue>
static bool zend_parse_parameters_long_wrapper(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value) {
	zval* object_zv;
	zend_long actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_writer_ce, 0, 0)
		Z_PARAM_LONG(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	object = WRITER_FROM_ZVAL(object_zv);
	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
static bool zend_parse_parameters_double_wrapper(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value) {
	zval* object_zv;
	double actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_writer_ce, 0, 0)
		Z_PARAM_DOUBLE(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	object = WRITER_FROM_ZVAL(object_zv);
	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
static bool zend_parse_parameters_double_array_wrapper(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, std::vector<TValue>& valueArray) {
	zval* objectZv;
	HashTable* valueArrayHt;
	zval* elementZv;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(objectZv, byte_buffer_writer_ce, 0, 0)
		Z_PARAM_ARRAY_HT(valueArrayHt)
		ZEND_PARSE_PARAMETERS_END_EX(return false);

	ZEND_HASH_FOREACH_VAL(valueArrayHt, elementZv) {
		if (Z_TYPE_P(elementZv) != IS_DOUBLE) {
			//TODO: give the correct array key when strings are used - I don't know how to do this in a non-awkward way currently
			zend_type_error("Array must contain only float, %s given at position %zu", zend_zval_type_name(elementZv), valueArray.size());
			return false;
		}
		TValue value = static_cast<TValue>(Z_DVAL_P(elementZv));
		valueArray.push_back(value);
	} ZEND_HASH_FOREACH_END();

	object = WRITER_FROM_ZVAL(objectZv);
	return true;
}

template<typename TValue>
static bool zend_parse_parameters_long_array_wrapper(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, std::vector<TValue>& valueArray) {
	zval* objectZv;
	HashTable* valueArrayHt;
	zval* elementZv;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(objectZv, byte_buffer_writer_ce, 0, 0)
		Z_PARAM_ARRAY_HT(valueArrayHt)
		ZEND_PARSE_PARAMETERS_END_EX(return false);

	ZEND_HASH_FOREACH_VAL(valueArrayHt, elementZv) {
		if (Z_TYPE_P(elementZv) != IS_LONG) {
			//TODO: give the correct array key when strings are used - I don't know how to do this in a non-awkward way currently
			zend_type_error("Array must contain only int, %s given at position %zu", zend_zval_type_name(elementZv), valueArray.size());
			return false;
		}
		TValue value = static_cast<TValue>(Z_LVAL_P(elementZv));
		valueArray.push_back(value);
	} ZEND_HASH_FOREACH_END();

	object = WRITER_FROM_ZVAL(objectZv);
	return true;
}

template<typename TValue>
using parseParametersFunc_t = bool (*)(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value);

template<typename TValue>
using writeTypeFunc_t = void (*)(zend_string*& buffer, size_t& offset, TValue value);

template<typename TValue, parseParametersFunc_t<TValue> parseParametersFunc, writeTypeFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	byte_buffer_writer_zend_object* object;

	//offsets beyond the end of the buffer are allowed, and result in automatic buffer extension
	if (!parseParametersFunc(execute_data, object, value)) {
		return;
	}

	writeTypeFunc(object->buffer, object->offset, value);
	if (object->offset > object->used) {
		object->used = object->offset;
	}
}

template<typename TValue>
using writeTypeArrayFunc_t = void (*)(zend_string*& buffer, size_t& offset, std::vector<TValue>& value);

template<typename TValue, parseParametersFunc_t<std::vector<TValue>> parseParametersFunc, writeTypeArrayFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeTypeArray(INTERNAL_FUNCTION_PARAMETERS) {
	std::vector<TValue> valueArray;
	byte_buffer_writer_zend_object* object;

	if (!parseParametersFunc(execute_data, object, valueArray)) {
		return;
	}

	//TODO: for fixed-size types with byte order == native, we can do a simple memcpy instead of this slow loop
	writeTypeFunc(object->buffer, object->offset, valueArray);
	if (object->offset > object->used) {
		object->used = object->offset;
	}
}

ZEND_NAMED_FUNCTION(pmmp_encoding_private_constructor) {
	//NOOP
}

#if PHP_VERSION_ID >= 80400
#define BC_ZEND_RAW_FENTRY(zend_name, name, arg_info) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL, NULL)
#else
#define BC_ZEND_RAW_FENTRY(zend_name, name, arg_info) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif

#define TYPE_ENTRIES(zend_name, native_type, read_type, read_result_wrapper, arg_info_read, write_parameter_wrapper, write_type, arg_info_write) \
	BC_ZEND_RAW_FENTRY( \
		"read" zend_name, \
		(zif_readType<native_type, read_type, read_result_wrapper<native_type>>), \
		arg_info_read \
	) \
	\
	BC_ZEND_RAW_FENTRY( \
		"write" zend_name, \
		(zif_writeType<native_type, write_parameter_wrapper<native_type>, write_type>), \
		arg_info_write \
	)

#define FIXED_TYPE_ENTRIES(zend_name, native_type, write_parameter_wrapper, read_result_wrapper, arg_info_read, arg_info_write, byte_order) \
	TYPE_ENTRIES( \
		zend_name, \
		native_type, \
		(readFixedSizeType<native_type, byte_order>), \
		read_result_wrapper, \
		arg_info_read, \
		write_parameter_wrapper, \
		(writeFixedSizeType<native_type, byte_order>), \
		arg_info_write \
	)

#define TYPE_ARRAY_ENTRIES(zend_name, native_type, read_complex_type, read_result_wrapper, write_parameter_wrapper, write_complex_type) \
	BC_ZEND_RAW_FENTRY( \
		"read" zend_name "Array", \
		(zif_readTypeArray<native_type, read_complex_type, read_result_wrapper<native_type>>), \
		arginfo_read_array \
	) \
	\
	BC_ZEND_RAW_FENTRY( \
		"write" zend_name "Array", \
		(zif_writeTypeArray<native_type, write_parameter_wrapper<native_type>, write_complex_type>), \
		arginfo_write_array \
	)

#define FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, read_result_wrapper, write_parameter_wrapper, byte_order) \
	TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		(readFixedSizeTypeArray<native_type, byte_order>), \
		read_result_wrapper, \
		write_parameter_wrapper, \
		(writeFixedSizeTypeArray<native_type, byte_order>) \
	)

#define COMPLEX_TYPE_ARRAY_ENTRIES(zend_name, native_type, read_element, read_result_wrapper, write_parameter_wrapper, write_element) \
	TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		(readComplexTypeArray<native_type, read_element>), \
		read_result_wrapper, \
		write_parameter_wrapper, \
		(writeComplexTypeArray<native_type, write_element>) \
	)

#define FIXED_INT_BASE_ENTRIES(zend_name, native_type, byte_order) \
	FIXED_TYPE_ENTRIES(zend_name, native_type, zend_parse_parameters_long_wrapper, zval_long_wrapper, arginfo_read_integer, arginfo_write_integer, byte_order) \
	\
	FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, return_long_array, zend_parse_parameters_long_array_wrapper, byte_order)

#define FIXED_INT_ENTRIES(zend_name, unsigned_native_type, signed_native_type, byte_order) \
	FIXED_INT_BASE_ENTRIES("Unsigned" zend_name, unsigned_native_type, byte_order) \
	\
	FIXED_INT_BASE_ENTRIES("Signed" zend_name, signed_native_type, byte_order)

#define FLOAT_ENTRIES(zend_name, native_type, byte_order) \
	FIXED_TYPE_ENTRIES(zend_name, native_type, zend_parse_parameters_double_wrapper, zval_double_wrapper, arginfo_read_float, arginfo_write_float, byte_order) \
	FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, return_double_array, zend_parse_parameters_double_array_wrapper, byte_order)

#define COMPLEX_INT_ENTRIES(zend_name, native_type, read_type, write_type) \
	TYPE_ENTRIES( \
		zend_name, \
		native_type, \
		read_type, \
		zval_long_wrapper, \
		arginfo_read_integer, \
		zend_parse_parameters_long_wrapper, \
		write_type, \
		arginfo_write_integer \
	) \
	COMPLEX_TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		read_type, \
		return_long_array, \
		zend_parse_parameters_long_array_wrapper, \
		write_type \
	)

//triad can't used readFixedSizeType because it's not a power of 2 bytes
#define TRIAD_ENTRIES(zend_name, unsigned_type, signed_type, byte_order) \
	COMPLEX_INT_ENTRIES("Unsigned" zend_name, unsigned_type, (readInt24<unsigned_type, byte_order>), (writeInt24<unsigned_type, byte_order>)) \
	COMPLEX_INT_ENTRIES("Signed" zend_name, signed_type, (readInt24<signed_type, byte_order>), (writeInt24<signed_type, byte_order>))

#define VARINT_ENTRIES(zend_name, unsigned_type, signed_type) \
	COMPLEX_INT_ENTRIES("Unsigned" zend_name, unsigned_type, (readUnsignedVarInt<unsigned_type>), (writeUnsignedVarInt<unsigned_type>)) \
	COMPLEX_INT_ENTRIES("Signed" zend_name, signed_type, (readSignedVarInt<unsigned_type, signed_type>), (writeSignedVarInt<unsigned_type, signed_type>))

#define BYTE_ENTRY(zend_name, native_type) \
	TYPE_ENTRIES(zend_name, native_type, (readByte<native_type>), zval_long_wrapper, arginfo_read_integer, zend_parse_parameters_long_wrapper, (writeByte<native_type>), arginfo_write_integer)

#define ENDIAN_ENTRIES(enum_case) \
	FIXED_INT_ENTRIES("Short", uint16_t, int16_t, enum_case) \
	FIXED_INT_ENTRIES("Int", uint32_t, int32_t, enum_case) \
	\
	/* Technically, PHP doesn't support unsigned longs in userland, however the functions are still useful for making code intent obvious */ \
	FIXED_INT_ENTRIES("Long", uint64_t, int64_t, enum_case) \
	\
	FLOAT_ENTRIES("Float", float, enum_case) \
	FLOAT_ENTRIES("Double", double, enum_case) \
	\
	TRIAD_ENTRIES("Triad", uint32_t, int32_t, enum_case)

ZEND_BEGIN_ARG_INFO_EX(empty_constructor_arg_info, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry byte_methods[] = {
	ZEND_NAMED_ME(__construct, pmmp_encoding_private_constructor, empty_constructor_arg_info, ZEND_ACC_PRIVATE)

	BYTE_ENTRY("Unsigned", uint8_t)
	BYTE_ENTRY("Signed", int8_t)

	PHP_FE_END
};

static zend_function_entry big_endian_methods[] = {
	ZEND_NAMED_ME(__construct, pmmp_encoding_private_constructor, empty_constructor_arg_info, ZEND_ACC_PRIVATE)

	ENDIAN_ENTRIES(ByteOrder::BigEndian)
	PHP_FE_END
};
static zend_function_entry little_endian_methods[] = {
	ZEND_NAMED_ME(__construct, pmmp_encoding_private_constructor, empty_constructor_arg_info, ZEND_ACC_PRIVATE)

	ENDIAN_ENTRIES(ByteOrder::LittleEndian)
	PHP_FE_END
};

static zend_function_entry varint_methods[] = {
	ZEND_NAMED_ME(__construct, pmmp_encoding_private_constructor, empty_constructor_arg_info, ZEND_ACC_PRIVATE)

	VARINT_ENTRIES("Int", uint32_t, int32_t)
	VARINT_ENTRIES("Long", uint64_t, int64_t)
	PHP_FE_END
};

void init_class_Types(void) {
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "Byte", byte_methods);
	ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_register_internal_class(&ce);

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "BE", big_endian_methods);
	ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_register_internal_class(&ce);

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "LE", little_endian_methods);
	ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_register_internal_class(&ce);

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "VarInt", varint_methods);
	ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_register_internal_class(&ce);
}
