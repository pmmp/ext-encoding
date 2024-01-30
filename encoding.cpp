/* encoding extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

extern "C" {
#include "php.h"
#include "ext/standard/info.h"
#include "php_encoding.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "stubs/BaseByteBuffer_arginfo.h"
#include "stubs/DataDecodeException_arginfo.h"
}
#include "ZendUtil.h"
#include <algorithm>
#include <iterator>
#include <type_traits>

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

enum class ByteOrder {
	BigEndian,
	LittleEndian,
#ifdef WORDS_BIGENDIAN
	Native = BigEndian
#else
	Native = LittleEndian
#endif
};

static zend_class_entry* data_decode_exception_ce;
static zend_class_entry* byte_buffer_ce;

static zend_object_handlers byte_buffer_zend_object_handlers;

typedef struct _byte_buffer_zend_object {
	zend_string* buffer;
	size_t read_offset;
	size_t write_offset;
	size_t used;
	zend_object std;
} byte_buffer_zend_object;

template<typename TValue>
union Flipper {
	char bytes[sizeof(TValue)];
	TValue value;
};

template<typename TValue>
static inline void zval_long_wrapper(zval* zv, TValue value) {
	ZVAL_LONG(zv, value);
}

template<typename TValue>
static inline void zval_double_wrapper(zval* zv, TValue value) {
	ZVAL_DOUBLE(zv, value);
}

#define BYTE_BUFFER_THIS() fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS))

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
static inline bool readByte(unsigned char* buffer, size_t used, size_t& offset, TValue& result) {
	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	result = *(reinterpret_cast<TValue*>(&buffer[offset]));

	offset += SIZE;
	return true;
}
template<typename TValue, ByteOrder byteOrder>
static inline bool readFixedSizeType(unsigned char* bytes, size_t used, size_t& offset, TValue& result) {

	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	Flipper<TValue> flipper;

	memcpy(flipper.bytes, &bytes[offset], sizeof(flipper.bytes));
	if (byteOrder != ByteOrder::Native) {
		std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
	}

	result = flipper.value;

	offset += SIZE;
	return true;
}

template<typename TValue, ByteOrder byteOrder>
static inline bool readInt24(unsigned char* bytes, size_t used, size_t& offset, TValue& result) {
	const size_t SIZE = 3;

	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	result = 0;
	if (byteOrder == ByteOrder::LittleEndian) {
		result |= bytes[offset];
		result |= bytes[offset + 1] << 8;
		result |= bytes[offset + 2] << 16;
	} else {
		result |= bytes[offset + 2];
		result |= bytes[offset + 1] << 8;
		result |= bytes[offset] << 16;
	}

	const size_t SIGNED_SHIFT = std::is_signed<TValue>::value ? (sizeof(TValue) - SIZE) * CHAR_BIT : 0;
	if (SIGNED_SHIFT > 0) {
		result = (result << SIGNED_SHIFT) >> SIGNED_SHIFT;
	}

	offset += SIZE;
	return true;
}

struct VarIntConstants {
	static const unsigned char BITS_PER_BYTE = 7u;

	template<size_t TYPE_BITS>
	static const unsigned char MAX_BYTES = TYPE_BITS / BITS_PER_BYTE + ((TYPE_BITS % BITS_PER_BYTE) > 0);

	static const unsigned char VALUE_MASK = static_cast<unsigned char>(~(1u << BITS_PER_BYTE));
	static const unsigned char MSB_MASK = static_cast<unsigned char>(1u << BITS_PER_BYTE);
};

template<typename TValue>
static inline bool readUnsignedVarInt(unsigned char *bytes, size_t used, size_t& offset, TValue &result) {
	const auto TYPE_BITS = sizeof(TValue) * CHAR_BIT;
	result = 0;
	for (unsigned int shift = 0; shift < TYPE_BITS; shift += VarIntConstants::BITS_PER_BYTE) {
		if (offset >= used) {
			zend_throw_exception(data_decode_exception_ce, "No bytes left in buffer", 0);
			return false;
		}

		auto byte = bytes[offset++];
		result |= (byte & VarIntConstants::VALUE_MASK) << shift;
		if ((byte & VarIntConstants::MSB_MASK) == 0) {
			return true;
		}
	}

	zend_throw_exception_ex(data_decode_exception_ce, 0, "VarInt did not terminate after %u bytes!", VarIntConstants::MAX_BYTES<TYPE_BITS>);
	return false;
}

template<typename TUnsignedValue, typename TSignedValue>
static inline bool readSignedVarInt(unsigned char* bytes, size_t used, size_t& offset, TSignedValue& result) {
	TUnsignedValue unsignedResult;
	if (!readUnsignedVarInt<TUnsignedValue>(bytes, used, offset, unsignedResult)) {
		return false;
	}

	TUnsignedValue mask = 0;
	if (unsignedResult & 1) {
		//we don't know the type of TUnsignedValue here so we can't just use ~0
		mask = ~mask;
	}

	result = static_cast<TSignedValue>((unsignedResult >> 1) ^ mask);
	return true;
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

static inline void extendBuffer(zend_string*& buffer, size_t offset, size_t usedBytes) {
	size_t requiredSize = offset + usedBytes;
	if (ZSTR_LEN(buffer) < requiredSize) {
		size_t doubleSize = ZSTR_LEN(buffer) * 2;
		buffer = zend_string_realloc(buffer, doubleSize > requiredSize ? doubleSize : requiredSize, 0);
		ZSTR_VAL(buffer)[ZSTR_LEN(buffer)] = '\0'; //make sure null terminator is always set, to stop sprintf reading out-of-bounds
	} else {
		buffer = zend_string_separate(buffer, 0);
	}
}

template<typename TValue>
static void writeByte(zend_string*& buffer, size_t& offset, TValue value) {
	extendBuffer(buffer, offset, sizeof(TValue));

	ZSTR_VAL(buffer)[offset] = *reinterpret_cast<char*>(&value);

	offset += sizeof(TValue);
}

template<typename TValue, ByteOrder byteOrder>
static void writeFixedSizeType(zend_string*& buffer, size_t& offset, TValue value) {
	extendBuffer(buffer, offset, sizeof(TValue));

	Flipper<TValue> flipper;
	flipper.value = value;

	if (byteOrder != ByteOrder::Native) {
		std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
	}

	memcpy(&ZSTR_VAL(buffer)[offset], flipper.bytes, sizeof(flipper.bytes));

	offset += sizeof(TValue);
}

template<typename TValue, ByteOrder byteOrder>
static void writeInt24(zend_string*& buffer, size_t& offset, TValue value) {
	const size_t SIZE = 3;
	extendBuffer(buffer, offset, SIZE);

	if (byteOrder == ByteOrder::LittleEndian) {
		ZSTR_VAL(buffer)[offset    ] = value & 0xff;
		ZSTR_VAL(buffer)[offset + 1] = (value >> 8) & 0xff;
		ZSTR_VAL(buffer)[offset + 2] = (value >> 16) & 0xff;
	} else {
		ZSTR_VAL(buffer)[offset    ] = (value >> 16) & 0xff;
		ZSTR_VAL(buffer)[offset + 1] = (value >> 8) & 0xff;
		ZSTR_VAL(buffer)[offset + 2] = value & 0xff;
	}

	offset += SIZE;
}

template<typename TValue>
static inline void writeUnsignedVarInt(zend_string*& buffer, size_t& offset, TValue value) {
	const auto TYPE_BITS = sizeof(TValue) * CHAR_BIT;
	char result[VarIntConstants::MAX_BYTES<TYPE_BITS>];

	TValue remaining = value;

	for (auto i = 0; i < VarIntConstants::MAX_BYTES<TYPE_BITS>; i++) {
		unsigned char nextByte = remaining & VarIntConstants::VALUE_MASK;

		TValue nextRemaining = remaining >> VarIntConstants::BITS_PER_BYTE;

		if (nextRemaining == 0) {
			result[i] = nextByte;

			auto usedBytes = i + 1;
			extendBuffer(buffer, offset, usedBytes);
			memcpy(&ZSTR_VAL(buffer)[offset], &result[0], usedBytes);
			offset += usedBytes;

			return;
		}

		result[i] = nextByte | VarIntConstants::MSB_MASK;
		remaining = nextRemaining;
	}

	zend_value_error("Value too large to be encoded as a VarInt");
}

template<typename TUnsignedType, typename TSignedType>
static inline void writeSignedVarInt(zend_string*& buffer, size_t& offset, TSignedType value) {
	TUnsignedType mask = 0;
	if (value < 0) {
		//we don't know the type of TUnsignedType here, can't use ~0 directly (the compiler will optimise this anyway)
		mask = ~mask;
	}

	writeUnsignedVarInt<TUnsignedType>(buffer, offset, (static_cast<TUnsignedType>(value) << 1) ^ mask);
}

static void byte_buffer_init_properties(byte_buffer_zend_object* object, zend_string* buffer, size_t used, size_t read_offset, size_t write_offset) {
	object->buffer = buffer;
	zend_string_addref(buffer);
	object->read_offset = read_offset;
	object->write_offset = write_offset;
	object->used = used;
}

static zend_object* byte_buffer_new(zend_class_entry* ce) {
	auto object = alloc_custom_zend_object<byte_buffer_zend_object>(ce, &byte_buffer_zend_object_handlers);

	byte_buffer_init_properties(object, zend_empty_string, 0, 0, 0);

	return &object->std;
}

static zend_object* byte_buffer_clone(zend_object* object) {
	auto old_object = fetch_from_zend_object<byte_buffer_zend_object>(object);
	auto new_object = fetch_from_zend_object<byte_buffer_zend_object>(byte_buffer_new(object->ce));

	zend_objects_clone_members(&new_object->std, &old_object->std);

	byte_buffer_init_properties(new_object, old_object->buffer, old_object->used, old_object->read_offset, old_object->write_offset);

	return &new_object->std;
}

static void byte_buffer_free(zend_object* std) {
	auto object = fetch_from_zend_object<byte_buffer_zend_object>(std);

	zend_string_release_ex(object->buffer, 0);
}

static int byte_buffer_compare_objects(zval* obj1, zval* obj2) {
	if (Z_TYPE_P(obj1) == IS_OBJECT && Z_TYPE_P(obj2) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE_P(obj1), byte_buffer_ce) && instanceof_function(Z_OBJCE_P(obj2), byte_buffer_ce)) {
			auto object1 = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(obj1));
			auto object2 = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(obj2));

			if (object1->offset == object2->offset && object1->used == object2->used && zend_string_equals(object1->buffer, object2->buffer)) {
				return 0;
			}
		}
	}

	return 1;
}

#define BYTE_BUFFER_METHOD(name) PHP_METHOD(pmmp_encoding_BaseByteBuffer, name)

BYTE_BUFFER_METHOD(__construct) {
	zend_string* buffer = NULL;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(buffer)
	ZEND_PARSE_PARAMETERS_END();

	object = BYTE_BUFFER_THIS();
	if (object->buffer) {
		zend_string_release_ex(object->buffer, 0);
	}

	if (buffer == NULL) {
		buffer = zend_empty_string;
	}
	//read offset is placed at the start, and write offset at the end (to mirror PM BinaryStream behaviour)
	byte_buffer_init_properties(object, buffer, ZSTR_LEN(buffer), 0, ZSTR_LEN(buffer));
}

BYTE_BUFFER_METHOD(toString) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_STRINGL(ZSTR_VAL(object->buffer), object->used);
}

BYTE_BUFFER_METHOD(readByteArray) {
	zend_long zlength;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
	ZEND_PARSE_PARAMETERS_END();

	if (zlength < 0) {
		zend_value_error("Length cannot be negative");
		return;
	}
	if (zlength == 0) { //to mirror PM BinaryStream behaviour
		RETURN_STR(zend_empty_string);
	}

	size_t length = static_cast<size_t>(zlength);

	object = BYTE_BUFFER_THIS();

	if (object->used - object->read_offset < length) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", length, object->used - object->read_offset);
		return;
	}

	RETVAL_STRINGL(ZSTR_VAL(object->buffer) + object->read_offset, length);
	object->read_offset += length;
}

BYTE_BUFFER_METHOD(writeByteArray) {
	zend_string* value;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();


	object = BYTE_BUFFER_THIS();

	auto size = ZSTR_LEN(value);

	extendBuffer(object->buffer, object->write_offset, size);
	memcpy(ZSTR_VAL(object->buffer) + object->write_offset, ZSTR_VAL(value), size);
	object->write_offset += size;
	if (object->write_offset > object->used) {
		object->used = object->write_offset;
	}
}

#define GET_OFFSET(func_name, which_offset) BYTE_BUFFER_METHOD(func_name) { \
	zend_parse_parameters_none_throw(); \
	auto object = BYTE_BUFFER_THIS(); \
	RETURN_LONG(object->which_offset); \
}

#define SET_OFFSET(func_name, which_offset) BYTE_BUFFER_METHOD(func_name) { \
	zend_long offset; \
\
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1) \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END(); \
\
	auto object = BYTE_BUFFER_THIS(); \
	if (offset < 0 || static_cast<size_t>(offset) > object->used) { \
		zend_value_error("Offset must not be less than zero or greater than the buffer size"); \
		return; \
	} \
\
	object->which_offset = static_cast<size_t>(offset); \
}

GET_OFFSET(getReadOffset, read_offset);
GET_OFFSET(getWriteOffset, write_offset);

SET_OFFSET(setReadOffset, read_offset);
SET_OFFSET(setWriteOffset, write_offset);

BYTE_BUFFER_METHOD(getUsedLength) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_LONG(object->used);
}

BYTE_BUFFER_METHOD(getReservedLength) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_LONG(ZSTR_LEN(object->buffer));
}

BYTE_BUFFER_METHOD(reserve) {
	zend_long zlength;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
	ZEND_PARSE_PARAMETERS_END();

	if (zlength <= 0) {
		zend_value_error("Length must be greater than zero");
		return;
	}
	auto object = BYTE_BUFFER_THIS();
	extendBuffer(object->buffer, static_cast<size_t>(zlength), 0);
}

BYTE_BUFFER_METHOD(trim) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	if (ZSTR_LEN(object->buffer) > object->used) {
		object->buffer = zend_string_truncate(object->buffer, object->used, 0);
	}
}

BYTE_BUFFER_METHOD(rewind) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();

	//TODO: doubtful that rewinding both offsets at the same time is desirable
	object->read_offset = 0;
	object->write_offset = 0;
}

BYTE_BUFFER_METHOD(__serialize) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	array_init(return_value);
	add_assoc_stringl(return_value, "buffer", ZSTR_VAL(object->buffer), object->used);
	add_assoc_long(return_value, "read_offset", object->read_offset);
	add_assoc_long(return_value, "write_offset", object->write_offset);
}

static zval* fetch_serialized_property(HashTable* data, const char* name, int type) {
	zval* zv = zend_hash_str_find(data, ZEND_STRL(name));
	if (zv == NULL) {
		zend_throw_exception_ex(NULL, 0, "Serialized data is missing \"%s\"", name);
		return NULL;
	}
	if (Z_TYPE_P(zv) != type) {
		zend_throw_exception_ex(NULL, 0, "\"%s\" in serialized data should be of type %s, but have %s", name, zend_zval_type_name(zv), zend_get_type_by_const(type));
		return NULL;
	}

	return zv;
}

BYTE_BUFFER_METHOD(__unserialize) {
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zval* buffer = fetch_serialized_property(data, "buffer", IS_STRING);
	if (buffer == NULL) {
		return;
	}
	zval* read_offset = fetch_serialized_property(data, "read_offset", IS_LONG);
	if (read_offset == NULL) {
		return;
	}
	zval* write_offset = fetch_serialized_property(data, "write_offset", IS_LONG);
	if (write_offset == NULL) {
		return;
	}

	auto object = BYTE_BUFFER_THIS();

	byte_buffer_init_properties(object, Z_STR_P(buffer), Z_STRLEN_P(buffer), Z_LVAL_P(read_offset), Z_LVAL_P(write_offset));
}

BYTE_BUFFER_METHOD(__debugInfo) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	array_init(return_value);
	add_assoc_stringl(return_value, "buffer", ZSTR_VAL(object->buffer), object->used);
	add_assoc_long(return_value, "read_offset", object->read_offset);
	add_assoc_long(return_value, "write_offset", object->write_offset);
}

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(encoding)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Version", PHP_ENCODING_VERSION);
	php_info_print_table_header(2, "Experimental", "YES");
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(encoding)
{
#if defined(ZTS) && defined(COMPILE_DL_ENCODING)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

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

PHP_MINIT_FUNCTION(encoding) {
	data_decode_exception_ce = register_class_pmmp_encoding_DataDecodeException(spl_ce_RuntimeException);

	zend_class_entry *base_byte_buffer = register_class_pmmp_encoding_BaseByteBuffer();
	base_byte_buffer->create_object = byte_buffer_new;

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "pmmp\\encoding\\ByteBuffer", byte_buffer_methods);
	byte_buffer_ce = zend_register_internal_class_ex(&ce, base_byte_buffer);
	byte_buffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	byte_buffer_zend_object_handlers = *zend_get_std_object_handlers();
	byte_buffer_zend_object_handlers.offset = XtOffsetOf(byte_buffer_zend_object, std);
	byte_buffer_zend_object_handlers.clone_obj = byte_buffer_clone;
	byte_buffer_zend_object_handlers.free_obj = byte_buffer_free;
	byte_buffer_zend_object_handlers.compare = byte_buffer_compare_objects;

	return SUCCESS;
}

static const zend_module_dep module_dependencies[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};

/* {{{ encoding_module_entry */
zend_module_entry encoding_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL, /* ini_entries */
	module_dependencies,
	"encoding",					/* Extension name */
	NULL,					/* zend_function_entry */
	PHP_MINIT(encoding),			/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(encoding),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(encoding),			/* PHP_MINFO - Module info */
	PHP_ENCODING_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ENCODING
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(encoding)
#endif
