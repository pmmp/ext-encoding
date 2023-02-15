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
}
#include "ZendUtil.h"

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_integer, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_integer, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_float, 0, 0, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_float, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
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
	size_t offset;
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

static inline bool handleOffsetReferenceParameter(const zval* const zoffset, size_t& offset, const zend_string* const bytes, size_t defaultOffset, size_t used) {
	if (zoffset != NULL) {
		auto type = Z_TYPE_P(Z_REFVAL_P(zoffset));
		if (type != IS_LONG && type != IS_NULL) {
			zend_type_error("$offset expects int|null, %s given", zend_get_type_by_const(type));
			return false;
		}
		zend_long offsetLval = Z_LVAL_P(Z_REFVAL_P(zoffset));

		if (offsetLval < 0) {
			zend_value_error("$offset expects at least 0, %zd given", offsetLval);
			return false;
		}
		if (bytes != NULL && static_cast<size_t>(offsetLval) >= used) {
			zend_value_error("$offset must be less than the length (%zd) of the input string, %zd given", used, offsetLval);
			return false;
		}

		offset = static_cast<size_t>(offsetLval);
	} else {
		offset = defaultOffset;
	}

	return true;
}

static inline void setOffsetReferenceParameter(zval* const zoffset, const size_t offset, size_t& memberOffset, size_t &memberUsed) {
	if (zoffset != NULL) {
		ZEND_TRY_ASSIGN_REF_LONG(zoffset, static_cast<zend_long>(offset));
	} else {
		memberOffset = offset;
	}
	if (offset > memberUsed) {
		memberUsed = offset;
	}
}

template<typename TValue, bool (*readTypeFunc)(zend_string* bytes, size_t used, size_t& offset, TValue& result), void(*assignResult)(zval*, TValue)>
void ZEND_FASTCALL zif_readType(INTERNAL_FUNCTION_PARAMETERS) {
	zval* zoffset = NULL;
	size_t offset = 0;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END();


	object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	if (!handleOffsetReferenceParameter(zoffset, offset, object->buffer, object->offset, object->used)) {
		return;
	}

	TValue result;
	if (readTypeFunc(object->buffer, object->used, offset, result)) {
		setOffsetReferenceParameter(zoffset, offset, object->offset, object->used);
		assignResult(return_value, result);
	}
}

template<typename TValue>
static inline bool readByte(zend_string* buffer, size_t used, size_t& offset, TValue& result) {
	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	result = *(reinterpret_cast<TValue*>(&ZSTR_VAL(buffer)[offset]));

	offset += SIZE;
	return true;
}
template<typename TValue, ByteOrder byteOrder>
static inline bool readFixedSizeType(zend_string* bytes, size_t used, size_t& offset, TValue& result) {

	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	if (byteOrder == ByteOrder::Native) {
		result = *(reinterpret_cast<TValue*>(&ZSTR_VAL(bytes)[offset]));
	} else {
		//endian flip
		Flipper<TValue> flipper;

		for (unsigned int i = 0; i < sizeof(TValue); i++) {
			flipper.bytes[sizeof(TValue) - i - 1] = ZSTR_VAL(bytes)[i + offset];
		}

		result = flipper.value;
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

template<size_t TYPE_BITS, typename TValue>
static inline bool readUnsignedVarInt(zend_string *bytes, size_t used, size_t& offset, TValue &result) {
	result = 0;
	for (unsigned int shift = 0; shift < TYPE_BITS; shift += VarIntConstants::BITS_PER_BYTE) {
		if (offset >= used) {
			zend_throw_exception(data_decode_exception_ce, "No bytes left in buffer", 0);
			return false;
		}
		TValue byte = (TValue) ZSTR_VAL(bytes)[offset++];

		result |= ((byte & VarIntConstants::VALUE_MASK) << shift);
		if ((byte & VarIntConstants::MSB_MASK) == 0) {
			return true;
		}
	}

	zend_throw_exception_ex(data_decode_exception_ce, 0, "VarInt did not terminate after %u bytes!", VarIntConstants::MAX_BYTES<TYPE_BITS>);
	return false;
}

template<size_t TYPE_BITS, typename TUnsignedValue, typename TSignedValue>
static inline bool readSignedVarInt(zend_string* bytes, size_t used, size_t& offset, TSignedValue& result) {
	TUnsignedValue unsignedResult;
	if (!readUnsignedVarInt<TYPE_BITS, TUnsignedValue>(bytes, used, offset, unsignedResult)) {
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
static bool zend_parse_parameters_long_wrapper(zend_execute_data* execute_data, zval*& zoffset, TValue& value) {
	zend_long actualValue;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_LONG(actualValue)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
static bool zend_parse_parameters_double_wrapper(zend_execute_data* execute_data, zval*& zoffset, TValue& value) {
	double actualValue;
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_DOUBLE(actualValue)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END_EX(return false);

	value = static_cast<TValue>(actualValue);

	return true;
}

template<typename TValue>
using parseParametersFunc_t = bool (*)(zend_execute_data* execute_data, zval*& zoffset, TValue& value);

template<typename TValue>
using writeTypeFunc_t = zend_string* (*)(zend_string *buffer, size_t& offset, TValue value);

template<typename TValue, parseParametersFunc_t<TValue> parseParametersFunc, writeTypeFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	zval* zoffset = NULL;
	size_t offset;
	byte_buffer_zend_object* object;

	object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));

	//offsets beyond the end of the buffer are allowed, and result in automatic buffer extension
	if (!parseParametersFunc(execute_data, zoffset, value) || !handleOffsetReferenceParameter(zoffset, offset, NULL, object->offset, object->used)) {
		return;
	}

	object->buffer = writeTypeFunc(object->buffer, offset, value);
	setOffsetReferenceParameter(zoffset, offset, object->offset, object->used);
}

static inline zend_string* extendBuffer(zend_string* buffer, size_t offset, size_t usedBytes) {
	size_t requiredSize = offset + usedBytes;
	if (ZSTR_LEN(buffer) < requiredSize) {
		size_t doubleSize = ZSTR_LEN(buffer) * 2;
		buffer = zend_string_realloc(buffer, doubleSize > requiredSize ? doubleSize : requiredSize, 0);
		ZSTR_VAL(buffer)[ZSTR_LEN(buffer)] = '\0'; //make sure null terminator is always set, to stop sprintf reading out-of-bounds
	} else {
		buffer = zend_string_separate(buffer, 0);
	}

	return buffer;
}

static zend_string* writeByte(zend_string* buffer, size_t& offset, int8_t value) {
	buffer = extendBuffer(buffer, offset, sizeof(int8_t));

	ZSTR_VAL(buffer)[offset] = *reinterpret_cast<char*>(&value);

	offset += sizeof(int8_t);

	return buffer;
}

template<typename TValue, ByteOrder byteOrder>
static zend_string* writeFixedSizeType(zend_string* buffer, size_t& offset, TValue value) {
	buffer = extendBuffer(buffer, offset, sizeof(TValue));

	if (byteOrder == ByteOrder::Native) {
		memcpy(&ZSTR_VAL(buffer)[offset], reinterpret_cast<const char*>(&value), sizeof(TValue));
	} else {
		Flipper<TValue> flipper;
		flipper.value = value;

		for (unsigned int i = 0; i < sizeof(TValue); i++) {
			ZSTR_VAL(buffer)[i + offset] = flipper.bytes[sizeof(TValue) - i - 1];
		}
	}

	offset += sizeof(TValue);

	return buffer;
}

template<size_t TYPE_BITS, typename TValue>
static inline zend_string* writeUnsignedVarInt(zend_string* buffer, size_t& offset, TValue value) {
	char result[VarIntConstants::MAX_BYTES<TYPE_BITS>];

	TValue remaining = value;

	for (auto i = 0; i < VarIntConstants::MAX_BYTES<TYPE_BITS>; i++) {
		unsigned char nextByte = remaining & VarIntConstants::VALUE_MASK;

		TValue nextRemaining = remaining >> VarIntConstants::BITS_PER_BYTE;

		if (nextRemaining == 0) {
			result[i] = nextByte;

			auto usedBytes = i + 1;
			buffer = extendBuffer(buffer, offset, usedBytes);
			memcpy(&ZSTR_VAL(buffer)[offset], &result[0], usedBytes);
			offset += usedBytes;

			return buffer;
		}

		result[i] = nextByte | VarIntConstants::MSB_MASK;
		remaining = nextRemaining;
	}

	zend_value_error("Value too large to be encoded as a VarInt");
	return buffer;
}

template<size_t TYPE_BITS, typename TUnsignedType, typename TSignedType>
static inline zend_string* writeSignedVarInt(zend_string* buffer, size_t& offset, TSignedType value) {
	TUnsignedType mask = 0;
	if (value < 0) {
		//we don't know the type of TUnsignedType here, can't use ~0 directly (the compiler will optimise this anyway)
		mask = ~mask;
	}

	return writeUnsignedVarInt<TYPE_BITS, TUnsignedType>(buffer, offset, (static_cast<TUnsignedType>(value) << 1) ^ mask);
}

static zend_object* byte_buffer_new(zend_class_entry* ce) {
	auto object = alloc_custom_zend_object<byte_buffer_zend_object>(ce, &byte_buffer_zend_object_handlers);

	object->offset = 0;
	object->used = 0;
	object->buffer = zend_empty_string;
	return &object->std;
}

static zend_object* byte_buffer_clone(zend_object* object) {
	auto old_object = fetch_from_zend_object<byte_buffer_zend_object>(object);
	auto new_object = fetch_from_zend_object<byte_buffer_zend_object>(byte_buffer_new(object->ce));

	zend_objects_clone_members(&new_object->std, &old_object->std);

	new_object->buffer = old_object->buffer;
	zend_string_addref(new_object->buffer);
	new_object->offset = old_object->offset;

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

			if (object1->offset == object2->offset && zend_string_equals(object1->buffer, object2->buffer)) {
				return 0;
			}
		}
	}

	return 1;
}

ZEND_BEGIN_ARG_INFO_EX(ByteBuffer___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, __construct) {
	zend_string* buffer;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(buffer)
	ZEND_PARSE_PARAMETERS_END();

	object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	if (object->buffer) {
		zend_string_release_ex(object->buffer, 0);
	}

	object->buffer = buffer;
	object->used = ZSTR_LEN(buffer);
	zend_string_addref(buffer);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, toString) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	RETURN_STRINGL(ZSTR_VAL(object->buffer), object->used);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_readByteArray, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, readByteArray) {
	zval *zoffset = NULL;
	zend_long zlength;
	size_t offset = 0;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_LONG(zlength)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END();

	if (zlength < 0) {
		zend_value_error("Length cannot be negative");
		return;
	}
	if (zlength == 0) { //to mirror PM BinaryStream behaviour
		RETURN_STR(zend_empty_string);
	}

	size_t length = static_cast<size_t>(zlength);

	object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	if (!handleOffsetReferenceParameter(zoffset, offset, object->buffer, object->offset, object->used)) {
		return;
	}

	if (object->used - offset < length) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", length, object->used - offset);
		return;
	}

	RETVAL_STRINGL(ZSTR_VAL(object->buffer) + offset, length);
	offset += length;
	setOffsetReferenceParameter(zoffset, offset, object->offset, object->used);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_writeByteArray, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, writeByteArray) {
	zend_string* value;
	zval* zoffset = NULL;
	size_t offset;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_STR(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END();


	object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	if (!handleOffsetReferenceParameter(zoffset, offset, NULL, object->offset, object->used)) {
		return;
	}

	object->buffer = extendBuffer(object->buffer, offset, ZSTR_LEN(value));
	memcpy(ZSTR_VAL(object->buffer) + offset, ZSTR_VAL(value), ZSTR_LEN(value));
	setOffsetReferenceParameter(zoffset, offset + ZSTR_LEN(value), object->offset, object->used);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_getOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, getOffset) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	RETURN_LONG(object->offset);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_setOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, setOffset) {
	zend_long offset;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	if (offset < 0 || static_cast<size_t>(offset) >= object->used) {
		zend_value_error("Offset must not be less than zero or greater than the buffer size");
		return;
	}

	object->offset = static_cast<size_t>(offset);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_getReserved, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, getReserved) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	RETURN_LONG(ZSTR_LEN(object->buffer));
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, reserve) {
	zend_long zlength;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
	ZEND_PARSE_PARAMETERS_END();

	if (zlength <= 0) {
		zend_value_error("Length must be greater than zero");
		return;
	}
	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	object->buffer = extendBuffer(object->buffer, static_cast<size_t>(zlength), 0);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_trim, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, trim) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	object->buffer = zend_string_truncate(object->buffer, object->used, 0);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, rewind) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	object->offset = 0;
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, __serialize) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	array_init(return_value);
	add_assoc_str(return_value, "buffer", object->buffer);
	add_assoc_long(return_value, "offset", object->offset);
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, __unserialize) {
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zval* buffer = zend_hash_str_find(data, ZEND_STRL("buffer"));
	if (buffer == NULL || Z_TYPE_P(buffer) != IS_STRING) {
		zend_throw_exception(NULL, "Buffer type in serialized data is not a string", 0);
		return;
	}
	zval* offset = zend_hash_str_find(data, ZEND_STRL("offset"));
	if (offset == NULL || Z_TYPE_P(offset) != IS_LONG) {
		zend_throw_exception(NULL, "Offset type in serialized data is not an int", 0);
		return;
	}

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	object->buffer = Z_STR_P(buffer);
	zend_string_addref(object->buffer);

	object->offset = static_cast<size_t>(Z_LVAL_P(offset));
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ByteBuffer___debugInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(ByteBuffer, __debugInfo) {
	zend_parse_parameters_none_throw();

	auto object = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS));
	array_init(return_value);
	add_assoc_str(return_value, "buffer", object->buffer);
	add_assoc_long(return_value, "offset", object->offset);
}

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(encoding)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "encoding support", "enabled");
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
	ZEND_RAW_FENTRY(zend_name "LE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::LittleEndian>), result_wrapper>), arg_info, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY(zend_name "BE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::BigEndian>), result_wrapper>), arg_info, ZEND_ACC_PUBLIC)

#define READ_VARINT_FENTRY(size, size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("readUnsignedVar" size_name, (zif_readType<unsigned_type, (readUnsignedVarInt<size, unsigned_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("readSignedVar" size_name, (zif_readType<signed_type, (readSignedVarInt<size, unsigned_type, signed_type>), zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC)

#define WRITE_FIXED_TYPE_FENTRY(zend_name, native_type, parse_parameters_wrapper, arg_info) \
	ZEND_RAW_FENTRY(zend_name "LE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::LittleEndian>)>), arg_info, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY(zend_name "BE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::BigEndian>)>), arg_info, ZEND_ACC_PUBLIC)

#define WRITE_VARINT_FENTRY(size, size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("writeUnsignedVar" size_name, (zif_writeType<unsigned_type, zend_parse_parameters_long_wrapper<unsigned_type>, (writeUnsignedVarInt<size, unsigned_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC) \
	ZEND_RAW_FENTRY("writeSignedVar" size_name, (zif_writeType<signed_type, zend_parse_parameters_long_wrapper<signed_type>, (writeSignedVarInt<size, unsigned_type, signed_type>)>), arginfo_write_integer, ZEND_ACC_PUBLIC)

static zend_function_entry byte_buffer_methods[] = {
	ZEND_RAW_FENTRY("readUnsignedByte", (zif_readType<uint8_t, readByte<uint8_t>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("readSignedByte", (zif_readType<int8_t, readByte<int8_t>, zval_long_wrapper>), arginfo_read_integer, ZEND_ACC_PUBLIC)

	READ_FIXED_TYPE_FENTRY("readUnsignedShort", uint16_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedShort", int16_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readUnsignedInt", uint32_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedInt", int32_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedLong", uint64_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readFloat", float, zval_double_wrapper, arginfo_read_float)
	READ_FIXED_TYPE_FENTRY("readDouble", double, zval_double_wrapper, arginfo_read_float)

	READ_VARINT_FENTRY(32, "Int", uint32_t, int32_t)
	READ_VARINT_FENTRY(64, "Long", uint64_t, int64_t)

	ZEND_RAW_FENTRY("writeByte", (zif_writeType<int8_t, zend_parse_parameters_long_wrapper<int8_t>, writeByte>), arginfo_write_integer, ZEND_ACC_PUBLIC)
	WRITE_FIXED_TYPE_FENTRY("writeShort", int16_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeInt", int32_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeLong", int64_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeFloat", float, zend_parse_parameters_double_wrapper, arginfo_write_float)
	WRITE_FIXED_TYPE_FENTRY("writeDouble", double, zend_parse_parameters_double_wrapper, arginfo_write_float)

	WRITE_VARINT_FENTRY(32, "Int", uint32_t, int32_t)
	WRITE_VARINT_FENTRY(64, "Long", uint64_t, int64_t)

	PHP_ME(ByteBuffer, __construct, ByteBuffer___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(ByteBuffer, toString, ByteBuffer_toString, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, readByteArray, ByteBuffer_readByteArray, ZEND_ACC_PUBLIC)
	PHP_ME(ByteBuffer, writeByteArray, ByteBuffer_writeByteArray, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, getOffset, ByteBuffer_getOffset, ZEND_ACC_PUBLIC)
	PHP_ME(ByteBuffer, setOffset, ByteBuffer_setOffset, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, getReserved, ByteBuffer_getReserved, ZEND_ACC_PUBLIC)
	PHP_ME(ByteBuffer, reserve, ByteBuffer_reserve, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, trim, ByteBuffer_trim, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, rewind, ByteBuffer_rewind, ZEND_ACC_PUBLIC)

	PHP_ME(ByteBuffer, __serialize, ByteBuffer___serialize, ZEND_ACC_PUBLIC)
	PHP_ME(ByteBuffer, __unserialize, ByteBuffer___unserialize, ZEND_ACC_PUBLIC)
	PHP_ME(ByteBuffer, __debugInfo, ByteBuffer___debugInfo, ZEND_ACC_PUBLIC)

	PHP_FE_END
};

PHP_MINIT_FUNCTION(encoding) {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "DataDecodeException", NULL);
	data_decode_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);
	data_decode_exception_ce->ce_flags |= ZEND_ACC_FINAL;

	INIT_CLASS_ENTRY(ce, "ByteBuffer", byte_buffer_methods);
	byte_buffer_ce = zend_register_internal_class(&ce);
	byte_buffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	byte_buffer_ce->create_object = byte_buffer_new;

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
