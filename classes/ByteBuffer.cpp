extern "C" {
#include "php.h"
}
#include "BaseByteBuffer.h"
#include "DataDecodeException.h"
#include "../Serializers.h"

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_integer, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_integer, 0, 1, IS_VOID, 0)
ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_float, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_float, 0, 1, IS_VOID, 0)
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
	byte_buffer_zend_object* object;

	zend_parse_parameters_none_throw();

	object = BYTE_BUFFER_THIS();

	TValue result;
	auto bytes = reinterpret_cast<unsigned char*>(ZSTR_VAL(object->buffer));
	if (readTypeFunc(bytes, object->used, object->read_offset, result)) {
		assignResult(return_value, result);
	}
}


template<typename TValue>
static bool zend_parse_parameters_long_wrapper(zend_execute_data* execute_data, TValue& value) {
	zend_long actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(actualValue)
		ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
static bool zend_parse_parameters_double_wrapper(zend_execute_data* execute_data, TValue& value) {
	double actualValue;
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_DOUBLE(actualValue)
		ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
using parseParametersFunc_t = bool (*)(zend_execute_data* execute_data, TValue& value);

template<typename TValue>
using writeTypeFunc_t = void (*)(zend_string*& buffer, size_t& offset, TValue value);

template<typename TValue, parseParametersFunc_t<TValue> parseParametersFunc, writeTypeFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	byte_buffer_zend_object* object;

	object = BYTE_BUFFER_THIS();

	//offsets beyond the end of the buffer are allowed, and result in automatic buffer extension
	if (!parseParametersFunc(execute_data, value)) {
		return;
	}

	writeTypeFunc(object->buffer, object->write_offset, value);
	if (object->write_offset > object->used) {
		object->used = object->write_offset;
	}
}

#define READ_FIXED_TYPE_FENTRY(zend_name, native_type, result_wrapper, arg_info) \
	ZEND_RAW_FENTRY("read" zend_name "LE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::LittleEndian>), result_wrapper>), arg_info, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("read" zend_name "BE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::BigEndian>), result_wrapper>), arg_info, ZEND_ACC_PUBLIC)

#define READ_VARINT_FENTRY(size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("readUnsignedVar" size_name, (zif_readType<unsigned_type, (readUnsignedVarInt<unsigned_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("readSignedVar" size_name, (zif_readType<signed_type, (readSignedVarInt<unsigned_type, signed_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC)

#define WRITE_FIXED_TYPE_FENTRY(zend_name, native_type, parse_parameters_wrapper, arg_info) \
	ZEND_RAW_FENTRY("write" zend_name "LE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::LittleEndian>)>), arg_info, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("write" zend_name "BE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::BigEndian>)>), arg_info, ZEND_ACC_PUBLIC)

#define WRITE_VARINT_FENTRY(size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("writeUnsignedVar" size_name, (zif_writeType<unsigned_type, zend_parse_parameters_long_wrapper<unsigned_type>, (writeUnsignedVarInt<unsigned_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("writeSignedVar" size_name, (zif_writeType<signed_type, zend_parse_parameters_long_wrapper<signed_type>, (writeSignedVarInt<unsigned_type, signed_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC)

#define READ_WRITE_LONG_ENTRY(zend_name, unsigned_native, signed_native) \
	READ_FIXED_TYPE_FENTRY("Unsigned" zend_name, unsigned_native, zval_long_wrapper, arginfo_read_integer) \
	READ_FIXED_TYPE_FENTRY("Signed" zend_name, signed_native, zval_long_wrapper, arginfo_read_integer) \
	WRITE_FIXED_TYPE_FENTRY("Unsigned" zend_name, unsigned_native, zend_parse_parameters_long_wrapper, arginfo_write_integer) \
	WRITE_FIXED_TYPE_FENTRY("Signed" zend_name, signed_native, zend_parse_parameters_long_wrapper, arginfo_write_integer)

#define READ_WRITE_FLOAT_ENTRY(zend_name, native_type) \
	READ_FIXED_TYPE_FENTRY(zend_name, native_type, zval_double_wrapper, arginfo_read_float) \
	WRITE_FIXED_TYPE_FENTRY(zend_name, native_type, zend_parse_parameters_double_wrapper, arginfo_write_float)

#define READ_WRITE_VARINT_ENTRY(zend_name, unsigned_type, signed_type) \
	READ_VARINT_FENTRY(zend_name, unsigned_type, signed_type) \
	WRITE_VARINT_FENTRY(zend_name, unsigned_type, signed_type)

#define READ_WRITE_BYTE_ENTRY(zend_name, native_type) \
	ZEND_RAW_FENTRY("read" zend_name, (zif_readType<native_type, readByte<native_type>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("write" zend_name, (zif_writeType<native_type, zend_parse_parameters_long_wrapper<native_type>, writeByte<native_type>>), arginfo_write_integer, ZEND_ACC_PUBLIC)

#define READ_TRIAD_ENTRY(zend_name, native_type) \
	ZEND_RAW_FENTRY("read" zend_name "BE", (zif_readType<native_type, readInt24<native_type, ByteOrder::BigEndian>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("read" zend_name "LE", (zif_readType<native_type, readInt24<native_type, ByteOrder::LittleEndian>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC)

#define WRITE_TRIAD_ENTRY(zend_name, native_type) \
	ZEND_RAW_FENTRY("write" zend_name "BE", (zif_writeType<native_type, zend_parse_parameters_long_wrapper<native_type>, writeInt24<native_type, ByteOrder::BigEndian>>), arginfo_write_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("write" zend_name "LE", (zif_writeType<native_type, zend_parse_parameters_long_wrapper<native_type>, writeInt24<native_type, ByteOrder::LittleEndian>>), arginfo_write_integer, ZEND_ACC_PUBLIC)

#define READ_WRITE_TRIAD_ENTRY(zend_name, native_type) \
	READ_TRIAD_ENTRY(zend_name, native_type) \
	WRITE_TRIAD_ENTRY(zend_name, native_type)

static zend_function_entry byte_buffer_methods[] = {
	READ_WRITE_BYTE_ENTRY("UnsignedByte", uint8_t)
	READ_WRITE_BYTE_ENTRY("SignedByte", int8_t)

	READ_WRITE_LONG_ENTRY("Short", uint16_t, int16_t)
	READ_WRITE_LONG_ENTRY("Int", uint32_t, int32_t)

	READ_FIXED_TYPE_FENTRY("SignedLong", int64_t, zval_long_wrapper, arginfo_read_integer)
	WRITE_FIXED_TYPE_FENTRY("SignedLong", int64_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)

	READ_WRITE_FLOAT_ENTRY("Float", float)
	READ_WRITE_FLOAT_ENTRY("Double", double)

	READ_WRITE_VARINT_ENTRY("Int", uint32_t, int32_t)
	READ_WRITE_VARINT_ENTRY("Long", uint64_t, int64_t)

	READ_WRITE_TRIAD_ENTRY("UnsignedTriad", uint32_t)
	READ_WRITE_TRIAD_ENTRY("SignedTriad", int32_t)

	PHP_FE_END
};

zend_class_entry* init_class_ByteBuffer(void) {
	zend_class_entry* base_byte_buffer_ce = init_class_BaseByteBuffer();
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "pmmp\\encoding\\ByteBuffer", byte_buffer_methods);
	zend_class_entry* byte_buffer_ce = zend_register_internal_class_ex(&ce, base_byte_buffer_ce);
	byte_buffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return byte_buffer_ce;
}
