extern "C" {
#include "php.h"
#include "Zend/zend_errors.h"
}
#include "ByteBufferReader.h"
#include "ByteBufferWriter.h"
#include "DataDecodeException.h"
#include "../Serializers.h"
#include <type_traits>
#include <vector>

#define ARG_INFOS(name, type_code) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_##name, 0, 1, type_code, 0) \
	ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferReader, 0) \
	ZEND_END_ARG_INFO() \
	\
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unpack_##name, 0, 1, type_code, 0) \
	ZEND_ARG_TYPE_INFO(0, bytes, IS_STRING, 0) \
	ZEND_END_ARG_INFO() \
	\
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_##name, 0, 2, IS_VOID, 0) \
	ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferWriter, 0) \
	ZEND_ARG_TYPE_INFO(0, value, type_code, 0) \
	ZEND_END_ARG_INFO() \
	\
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pack_##name, 0, 1, IS_STRING, 0) \
	ZEND_ARG_TYPE_INFO(0, value, type_code, 0) \
	ZEND_END_ARG_INFO()

ARG_INFOS(zend_long, IS_LONG)
ARG_INFOS(double, IS_DOUBLE)

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_array, 0, 2, IS_ARRAY, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferReader, 0)
ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_array, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBufferWriter, 0)
ZEND_ARG_ARRAY_INFO(0, values, 0)
ZEND_END_ARG_INFO()


#if PHP_VERSION_ID >= 80400
static const char* read_zend_long_array_doc_comment = "/**\n\t * @return int[]\n\t * @phpstan-return list<int>\n\t * @throws DataDecodeException\n\t */";
static const char* read_double_array_doc_comment = "/**\n\t * @return float[]\n\t * @phpstan-return list<float>\n\t * @throws DataDecodeException\n\t */";
static const char* write_zend_long_array_doc_comment = "/**\n\t * @param int[] $values\n\t * @phpstan-param list<int> $values\n\t */";
static const char* write_double_array_doc_comment = "/**\n\t * @param float[] $values\n\t * @phpstan-param list<float> $values\n\t */";
static const char* read_generic_doc_comment = "/** @throws DataDecodeException */";
#endif

template<typename TValue, typename TZendValue>
static inline void assignZval(zval* zv, TValue value, std::type_identity<TZendValue> zendType) = delete;

template<typename TValue>
static inline void assignZval(zval* zv, TValue value, std::type_identity<zend_long> zendType) {
	//TODO: shouldn't this cast the type before returning? Probably implicit casting going on here
	ZVAL_LONG(zv, value);
}

template<typename TValue>
static inline void assignZval(zval* zv, TValue value, std::type_identity<double> zendType) {
	ZVAL_DOUBLE(zv, value);
}

template<typename TValue, typename TZendValue>
static inline void assignZvalArray(zval* zv, std::vector<TValue>& valueArray, std::type_identity<TZendValue> zendType) = delete;

template<typename TValue>
static inline void assignZvalArray(zval* zv, std::vector<TValue>& valueArray, std::type_identity<zend_long> zendType) {
	array_init_size(zv, valueArray.size());

	for (size_t i = 0; i < valueArray.size(); i++) {
		add_next_index_long(zv, static_cast<zend_long>(valueArray[i]));
	}
}

template<typename TValue>
static inline void assignZvalArray(zval* zv, std::vector<TValue>& valueArray, std::type_identity<double> zendType) {
	array_init_size(zv, valueArray.size());

	for (size_t i = 0; i < valueArray.size(); i++) {
		add_next_index_double(zv, static_cast<double>(valueArray[i]));
	}
}

template<typename TValue>
using readTypeFunc_t = bool (*)(unsigned char* bytes, size_t used, size_t& offset, TValue& result);

template<typename TValue, readTypeFunc_t<TValue> readTypeFunc, typename TZendValue>
inline void readTypeCommon(INTERNAL_FUNCTION_PARAMETERS, byte_buffer_reader_t& reader) {
	TValue result;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(reader.buffer));
	if (readTypeFunc(bytes, ZSTR_LEN(reader.buffer), reader.offset, result)) {
		assignZval(return_value, result, std::type_identity<TZendValue>{});
	}
}

template<typename TValue, readTypeFunc_t<TValue> readTypeFunc, typename TZendValue>
void ZEND_FASTCALL zif_readType(INTERNAL_FUNCTION_PARAMETERS) {
	zval* object_zv;
	byte_buffer_reader_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_reader_ce, 0, 0)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	object = READER_FROM_ZVAL(object_zv);

	readTypeCommon<TValue, readTypeFunc, TZendValue>(INTERNAL_FUNCTION_PARAM_PASSTHRU, object->reader);
}

template<typename TValue, readTypeFunc_t<TValue> readTypeFunc, typename TZendValue>
void ZEND_FASTCALL zif_unpackType(INTERNAL_FUNCTION_PARAMETERS) {
	byte_buffer_reader_t reader = { 0 };

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(reader.buffer)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	//TODO: we could allow passing the offset? but keep it simple for now
	reader.offset = 0;

	readTypeCommon<TValue, readTypeFunc, TZendValue>(INTERNAL_FUNCTION_PARAM_PASSTHRU, reader);
}


template<typename TValue>
using readTypeArrayFunc_t = bool (*) (unsigned char* bytes, size_t used, size_t& offset, size_t count, std::vector<TValue>& resultArray);

template<typename TValue, readTypeArrayFunc_t<TValue> readTypeArray, typename TZendValue>
void ZEND_FASTCALL zif_readTypeArray(INTERNAL_FUNCTION_PARAMETERS) {
	zval* object_zv;
	zend_long zcount;
	byte_buffer_reader_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_reader_ce, 0, 0)
		Z_PARAM_LONG(zcount)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	object = READER_FROM_ZVAL(object_zv);

	if (zcount < 1) {
		zend_value_error("Count must be at least 1");
		return;
	}
	size_t count = static_cast<size_t>(zcount);

	std::vector<TValue> resultArray;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(object->reader.buffer));
	if (readTypeArray(bytes, ZSTR_LEN(object->reader.buffer), object->reader.offset, count, resultArray)) {
		assignZvalArray(return_value, resultArray, std::type_identity<TZendValue>{});
	}
}

//this must be reimplemented for any new zend types handled, because ZPP macros can't be easily templated
template<typename TValue, typename TZendValue>
bool parseWriteTypeParams(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value, std::type_identity<TZendValue> zend_type) = delete;

template<typename TValue>
bool parseWriteTypeParams(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value, std::type_identity<zend_long> zend_type) {
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
bool parseWriteTypeParams(zend_execute_data* execute_data, byte_buffer_writer_zend_object*& object, TValue& value, std::type_identity<double> zend_type) {
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
using writeTypeFunc_t = void (*)(zend_string*& buffer, size_t& offset, TValue value);

template<typename TValue, writeTypeFunc_t<TValue> writeTypeFunc>
inline void writeTypeCommon(INTERNAL_FUNCTION_PARAMETERS, byte_buffer_writer_t& writer, TValue value) {
	writeTypeFunc(writer.buffer, writer.offset, value);
	if (writer.offset > writer.used) {
		writer.used = writer.offset;
	}
}

template<typename TValue, writeTypeFunc_t<TValue> writeTypeFunc, typename TZendValue>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	byte_buffer_writer_zend_object* object;

	if (!parseWriteTypeParams(execute_data, object, value, std::type_identity<TZendValue>{})) {
		return;
	}

	writeTypeCommon<TValue, writeTypeFunc>(INTERNAL_FUNCTION_PARAM_PASSTHRU, object->writer, value);
}

template<typename TValue, typename TZendValue>
bool parsePackTypeParams(zend_execute_data* execute_data, TValue& value, std::type_identity<TZendValue> zend_type) = delete;

template<typename TValue>
bool parsePackTypeParams(zend_execute_data* execute_data, TValue& value, std::type_identity<zend_long> zend_type) {
	zend_long actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);
	return true;
}

template<typename TValue>
bool parsePackTypeParams(zend_execute_data* execute_data, TValue& value, std::type_identity<double> zend_type) {
	double actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_DOUBLE(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);
	return true;
}

template<typename TValue, writeTypeFunc_t<TValue> writeTypeFunc, typename TZendValue>
void ZEND_FASTCALL zif_packType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	byte_buffer_writer_t writer = { 0 };

	if (!parsePackTypeParams(execute_data, value, std::type_identity<TZendValue>{})) {
		return;
	}

	writer.buffer = zend_empty_string;

	writeTypeCommon<TValue, writeTypeFunc>(INTERNAL_FUNCTION_PARAM_PASSTHRU, writer, value);

	RETVAL_STRINGL(ZSTR_VAL(writer.buffer), writer.used);
	zend_string_release_ex(writer.buffer, 0);
}


template<typename TValue, typename TZendValue>
bool typeHashTableToArray(HashTable* valueArrayHt, std::vector<TValue>& valueArray, std::type_identity<TZendValue> zendType) = delete;

template<typename TValue>
bool typeHashTableToArray(HashTable* valueArrayHt, std::vector<TValue>& valueArray, std::type_identity<zend_long> zendType) {
	zval* elementZv;
	ZEND_HASH_FOREACH_VAL(valueArrayHt, elementZv) {
		if (Z_TYPE_P(elementZv) != IS_LONG) {
			//TODO: give the correct array key when strings are used - I don't know how to do this in a non-awkward way currently
			zend_type_error("Array must contain only int, %s given at position %zu", zend_zval_type_name(elementZv), valueArray.size());
			return false;
		}
		TValue value = static_cast<TValue>(Z_LVAL_P(elementZv));
		valueArray.push_back(value);
	} ZEND_HASH_FOREACH_END();
	return true;
}

template<typename TValue>
bool typeHashTableToArray(HashTable* valueArrayHt, std::vector<TValue>& valueArray, std::type_identity<double> zendType) {
	zval* elementZv;
	ZEND_HASH_FOREACH_VAL(valueArrayHt, elementZv) {
		if (Z_TYPE_P(elementZv) != IS_DOUBLE) {
			//TODO: give the correct array key when strings are used - I don't know how to do this in a non-awkward way currently
			zend_type_error("Array must contain only float, %s given at position %zu", zend_zval_type_name(elementZv), valueArray.size());
			return false;
		}
		TValue value = static_cast<TValue>(Z_DVAL_P(elementZv));
		valueArray.push_back(value);
	} ZEND_HASH_FOREACH_END();
	return true;
}

template<typename TValue>
using writeTypeArrayFunc_t = void (*)(zend_string*& buffer, size_t& offset, std::vector<TValue>& value);

template<typename TValue, writeTypeArrayFunc_t<TValue> writeTypeFunc, typename TZendValue>
void ZEND_FASTCALL zif_writeTypeArray(INTERNAL_FUNCTION_PARAMETERS) {
	byte_buffer_writer_zend_object* object;
	zval* objectZv;
	HashTable* valueArrayHt;
	
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(objectZv, byte_buffer_writer_ce, 0, 0)
		Z_PARAM_ARRAY_HT(valueArrayHt)
	ZEND_PARSE_PARAMETERS_END();

	object = WRITER_FROM_ZVAL(objectZv);

	std::vector<TValue> valueArray;

	if (!typeHashTableToArray(valueArrayHt, valueArray, std::type_identity<TZendValue>{})) {
		return;
	}

	writeTypeFunc(object->writer.buffer, object->writer.offset, valueArray);
	if (object->writer.offset > object->writer.used) {
		object->writer.used = object->writer.offset;
	}
}

ZEND_NAMED_FUNCTION(pmmp_encoding_private_constructor) {
	//NOOP
}

#if PHP_VERSION_ID >= 80400
#define BC_ZEND_RAW_FENTRY(zend_name, name, arg_info) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL, NULL)
#define BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT(zend_name, name, arg_info, doc_comment) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL, doc_comment)
#else
#define BC_ZEND_RAW_FENTRY(zend_name, name, arg_info) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#define BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT(zend_name, name, arg_info, doc_comment) ZEND_RAW_FENTRY(zend_name, name, arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif

#define TYPE_ENTRIES(type_name, native_type, zend_type, read_type_func, write_type_func) \
	BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT( \
		"read" type_name, \
		(zif_readType<native_type, read_type_func, zend_type>), \
		arginfo_read_##zend_type, \
		read_generic_doc_comment \
	) \
	\
	BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT( \
		"unpack" type_name, \
		(zif_unpackType<native_type, read_type_func, zend_type>), \
		arginfo_unpack_##zend_type, \
		read_generic_doc_comment \
	) \
	BC_ZEND_RAW_FENTRY( \
		"write" type_name, \
		(zif_writeType<native_type, write_type_func, zend_type>), \
		arginfo_write_##zend_type \
	) \
	BC_ZEND_RAW_FENTRY( \
		"pack" type_name, \
		(zif_packType<native_type, write_type_func, zend_type>), \
		arginfo_pack_##zend_type \
	)

#define FIXED_TYPE_ENTRIES(type_name, native_type, zend_type, byte_order) \
	TYPE_ENTRIES( \
		type_name, \
		native_type, \
		zend_type, \
		(readFixedSizeType<native_type, byte_order>), \
		(writeFixedSizeType<native_type, byte_order>) \
	)

#define TYPE_ARRAY_ENTRIES(zend_name, native_type, zend_type, read_complex_type, write_complex_type) \
	BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT( \
		"read" zend_name "Array", \
		(zif_readTypeArray<native_type, read_complex_type, zend_type>), \
		arginfo_read_array, \
		read_##zend_type##_array_doc_comment \
	) \
	BC_ZEND_RAW_FENTRY_WITH_DOC_COMMENT( \
		"write" zend_name "Array", \
		(zif_writeTypeArray<native_type, write_complex_type, zend_type>), \
		arginfo_write_array, \
		write_##zend_type##_array_doc_comment \
	)

#define FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, zend_type, byte_order) \
	TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		zend_type, \
		(readFixedSizeTypeArray<native_type, byte_order>), \
		(writeFixedSizeTypeArray<native_type, byte_order>) \
	)

#define COMPLEX_TYPE_ARRAY_ENTRIES(zend_name, native_type, zend_type, read_element, write_element) \
	TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		zend_type, \
		(readComplexTypeArray<native_type, read_element>), \
		(writeComplexTypeArray<native_type, write_element>) \
	)

#define FIXED_INT_BASE_ENTRIES(zend_name, native_type, byte_order) \
	FIXED_TYPE_ENTRIES(zend_name, native_type, zend_long, byte_order) \
	\
	FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, zend_long, byte_order)

#define FIXED_INT_ENTRIES(zend_name, unsigned_native_type, signed_native_type, byte_order) \
	FIXED_INT_BASE_ENTRIES("Unsigned" zend_name, unsigned_native_type, byte_order) \
	\
	FIXED_INT_BASE_ENTRIES("Signed" zend_name, signed_native_type, byte_order)

#define FLOAT_ENTRIES(zend_name, native_type, byte_order) \
	FIXED_TYPE_ENTRIES(zend_name, native_type, double, byte_order) \
	FIXED_TYPE_ARRAY_ENTRIES(zend_name, native_type, double, byte_order)

#define COMPLEX_INT_ENTRIES(zend_name, native_type, read_type, write_type) \
	TYPE_ENTRIES( \
		zend_name, \
		native_type, \
		zend_long, \
		read_type, \
		write_type \
	) \
	COMPLEX_TYPE_ARRAY_ENTRIES( \
		zend_name, \
		native_type, \
		zend_long, \
		read_type, \
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
	TYPE_ENTRIES(zend_name, native_type, zend_long, (readByte<native_type>), (writeByte<native_type>))

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
