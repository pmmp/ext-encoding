extern "C" {
#include "php.h"
}
#include "ByteBuffer.h"
#include "DataDecodeException.h"
#include "../Serializers.h"

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_integer, 0, 1, IS_LONG, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBuffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_integer, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBuffer, 0)
ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_float, 0, 1, IS_DOUBLE, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBuffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_float, 0, 2, IS_VOID, 0)
ZEND_ARG_OBJ_INFO(0, buffer, pmmp\\encoding\\ByteBuffer, 0)
ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

template<typename TValue>
static inline void zval_long_wrapper(zval* zv, TValue value) {
	ZVAL_LONG(zv, value);
}

template<typename TValue>
static inline void zval_double_wrapper(zval* zv, TValue value) {
	ZVAL_DOUBLE(zv, value);
}

template<typename TValue, bool (*readTypeFunc)(unsigned char* bytes, size_t used, size_t& offset, TValue& result), void(*assignResult)(zval*, TValue)>
void ZEND_FASTCALL zif_readType(INTERNAL_FUNCTION_PARAMETERS) {
	zval* object_zv;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_ce, 0, 0)
	ZEND_PARSE_PARAMETERS_END_EX(return);

	object = BYTE_BUFFER_FROM_ZVAL(object_zv);

	TValue result;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(object->buffer));
	if (readTypeFunc(bytes, object->used, object->read_offset, result)) {
		assignResult(return_value, result);
	}
}


template<typename TValue>
static bool zend_parse_parameters_long_wrapper(zend_execute_data* execute_data, byte_buffer_zend_object*& object, TValue& value) {
	zval* object_zv;
	zend_long actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_ce, 0, 0)
		Z_PARAM_LONG(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	object = BYTE_BUFFER_FROM_ZVAL(object_zv);
	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
static bool zend_parse_parameters_double_wrapper(zend_execute_data* execute_data, byte_buffer_zend_object*& object, TValue& value) {
	zval* object_zv;
	double actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(object_zv, byte_buffer_ce, 0, 0)
		Z_PARAM_DOUBLE(actualValue)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	object = BYTE_BUFFER_FROM_ZVAL(object_zv);
	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
using parseParametersFunc_t = bool (*)(zend_execute_data* execute_data, byte_buffer_zend_object*& object, TValue& value);

template<typename TValue>
using writeTypeFunc_t = void (*)(zend_string*& buffer, size_t& offset, TValue value);

template<typename TValue, parseParametersFunc_t<TValue> parseParametersFunc, writeTypeFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	byte_buffer_zend_object* object;

	//offsets beyond the end of the buffer are allowed, and result in automatic buffer extension
	if (!parseParametersFunc(execute_data, object, value)) {
		return;
	}

	writeTypeFunc(object->buffer, object->write_offset, value);
	if (object->write_offset > object->used) {
		object->used = object->write_offset;
	}
}

ZEND_NAMED_FUNCTION(pmmp_encoding_private_constructor) {
	//NOOP
}

#define FIXED_TYPE_ENTRIES(zend_name, native_type, parse_parameters_wrapper, result_wrapper, arg_info_read, arg_info_write, byte_order) \
	ZEND_RAW_FENTRY("read" zend_name, (zif_readType<native_type, (readFixedSizeType<native_type, byte_order>), result_wrapper>), arg_info_read, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC) \
	ZEND_RAW_FENTRY("write" zend_name, (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, byte_order>)>), arg_info_write, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

//triad can't used readFixedSizeType because it's not a power of 2 bytes
#define TRIAD_ENTRIES(zend_name, native_type, byte_order) \
	ZEND_RAW_FENTRY("read" zend_name, (zif_readType<native_type, readInt24<native_type, byte_order>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC) \
	ZEND_RAW_FENTRY("write" zend_name, (zif_writeType<native_type, zend_parse_parameters_long_wrapper<native_type>, writeInt24<native_type, byte_order>>), arginfo_write_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

#define LONG_ENTRIES(zend_name, unsigned_native, signed_native, byte_order) \
	FIXED_TYPE_ENTRIES("Unsigned" zend_name, unsigned_native, zend_parse_parameters_long_wrapper, zval_long_wrapper, arginfo_read_integer, arginfo_write_integer, byte_order) \
	FIXED_TYPE_ENTRIES("Signed" zend_name, signed_native, zend_parse_parameters_long_wrapper, zval_long_wrapper, arginfo_read_integer, arginfo_write_integer, byte_order) \

#define FLOAT_ENTRIES(zend_name, native_type, byte_order) \
	FIXED_TYPE_ENTRIES(zend_name, native_type, zend_parse_parameters_double_wrapper, zval_double_wrapper, arginfo_read_float, arginfo_write_float, byte_order)

#define READ_VARINT_FENTRY(size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("readUnsigned" size_name, (zif_readType<unsigned_type, (readUnsignedVarInt<unsigned_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC) \
	ZEND_RAW_FENTRY("readSigned" size_name, (zif_readType<signed_type, (readSignedVarInt<unsigned_type, signed_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

#define WRITE_VARINT_FENTRY(size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("writeUnsigned" size_name, (zif_writeType<unsigned_type, zend_parse_parameters_long_wrapper<unsigned_type>, (writeUnsignedVarInt<unsigned_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC) \
	ZEND_RAW_FENTRY("writeSigned" size_name, (zif_writeType<signed_type, zend_parse_parameters_long_wrapper<signed_type>, (writeSignedVarInt<unsigned_type, signed_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

#define READ_WRITE_VARINT_ENTRY(zend_name, unsigned_type, signed_type) \
	READ_VARINT_FENTRY(zend_name, unsigned_type, signed_type) \
	WRITE_VARINT_FENTRY(zend_name, unsigned_type, signed_type)

#define READ_WRITE_BYTE_ENTRY(zend_name, native_type) \
	ZEND_RAW_FENTRY("read" zend_name, (zif_readType<native_type, readByte<native_type>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC) \
	ZEND_RAW_FENTRY("write" zend_name, (zif_writeType<native_type, zend_parse_parameters_long_wrapper<native_type>, writeByte<native_type>>), arginfo_write_integer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

#define ENDIAN_ENTRIES(enum_case) \
	LONG_ENTRIES("Short", uint16_t, int16_t, enum_case) \
	LONG_ENTRIES("Int", uint32_t, int32_t, enum_case) \
	\
	FIXED_TYPE_ENTRIES("SignedLong", int64_t, zend_parse_parameters_long_wrapper, zval_long_wrapper, arginfo_read_integer, arginfo_write_integer, enum_case) \
    \
	FLOAT_ENTRIES("Float", float, enum_case) \
	FLOAT_ENTRIES("Double", double, enum_case) \
	\
	TRIAD_ENTRIES("UnsignedTriad", uint32_t, enum_case) \
	TRIAD_ENTRIES("SignedTriad", int32_t, enum_case)

ZEND_BEGIN_ARG_INFO_EX(empty_constructor_arg_info, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry byte_methods[] = {
	ZEND_NAMED_ME(__construct, pmmp_encoding_private_constructor, empty_constructor_arg_info, ZEND_ACC_PRIVATE)

	READ_WRITE_BYTE_ENTRY("Unsigned", uint8_t)
	READ_WRITE_BYTE_ENTRY("Signed", int8_t)

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

	READ_WRITE_VARINT_ENTRY("Int", uint32_t, int32_t)
	READ_WRITE_VARINT_ENTRY("Long", uint64_t, int64_t)
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
