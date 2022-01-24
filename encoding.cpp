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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_integer, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, bytes, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_integer, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_read_float, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, bytes, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, offset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_write_float, 0, 1, IS_STRING, 0)
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

static inline bool handleOffsetReferenceParameter(const zval* const zoffset, zend_long& offset) {
	if (zoffset != NULL) {
		auto type = Z_TYPE_P(Z_REFVAL_P(zoffset));
		if (type != IS_LONG && type != IS_NULL) {
			zend_type_error("$offset expects int|null, %s given", zend_get_type_by_const(type));
			return false;
		}

		offset = static_cast<size_t>(Z_LVAL_P(Z_REFVAL_P(zoffset)));

		if (offset < 0) {
			zend_value_error("$offset expects at least 0, %zd given", offset);
			return false;
		}
	}

	return true;
}

static inline void setOffsetReferenceParameter(zval* const zoffset, const zend_long offset) {
	if (zoffset != NULL) {
		ZEND_TRY_ASSIGN_REF_LONG(zoffset, offset);
	}
}

template<typename TValue, bool (*readTypeFunc)(zend_string* bytes, zend_long& offset, TValue& result), void(*assignResult)(zval*, TValue)>
void ZEND_FASTCALL zif_readType(INTERNAL_FUNCTION_PARAMETERS) {
	zend_string* bytes;
	zval* zoffset = NULL;
	zend_long offset = 0;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_STR(bytes)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zoffset)
	ZEND_PARSE_PARAMETERS_END();

	if (!handleOffsetReferenceParameter(zoffset, offset)) {
		return;
	}

	TValue result;
	if (readTypeFunc(bytes, offset, result)) {
		setOffsetReferenceParameter(zoffset, offset);
		assignResult(return_value, result);
	}
}

template<typename TValue, ByteOrder byteOrder>
static inline bool readFixedSizeType(zend_string* bytes, zend_long& offset, TValue& result) {

	const auto SIZE = sizeof(TValue);
	if (ZSTR_LEN(bytes) - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, ZSTR_LEN(bytes) - offset);
		return false;
	}

	if (byteOrder == ByteOrder::Native) {
		result = *(reinterpret_cast<TValue*>(&ZSTR_VAL(bytes)[offset]));
	} else {
		//endian flip
		Flipper<TValue> flipper;

		for (auto i = 0; i < sizeof(TValue); i++) {
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
static inline bool readUnsignedVarInt(zend_string *bytes, zend_long &offset, TValue &result) {
	result = 0;
	for (auto shift = 0; shift < TYPE_BITS; shift += VarIntConstants::BITS_PER_BYTE) {
		if (offset >= ZSTR_LEN(bytes)) {
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
static inline bool readSignedVarInt(zend_string* bytes, zend_long& offset, TSignedValue& result) {
	TUnsignedValue unsignedResult;
	if (!readUnsignedVarInt<TYPE_BITS, TUnsignedValue>(bytes, offset, unsignedResult)) {
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
using writeTypeFunc_t = zend_string* (*)(TValue value);

template<typename TValue, parseParametersFunc_t<TValue> parseParametersFunc, writeTypeFunc_t<TValue> writeTypeFunc>
void ZEND_FASTCALL zif_writeType(INTERNAL_FUNCTION_PARAMETERS) {
	TValue value;
	zend_string* result;

	if (!parseParametersFunc(execute_data, value)) {
		return;
	}

	result = writeTypeFunc(value);
	if (result != NULL) { //assume that the function threw an error if null was returned
		RETURN_STR(result);
	}
}

template<typename TValue, ByteOrder byteOrder>
static zend_string* writeFixedSizeType(TValue value) {
	zend_string* result;

	if (byteOrder == ByteOrder::Native) {
		result = zend_string_init(reinterpret_cast<const char*>(&value), sizeof(TValue), 0);
	} else {
		Flipper<TValue> flipper;
		flipper.value = value;

		result = zend_string_alloc(sizeof(TValue), 0);

		for (auto i = 0; i < sizeof(TValue); i++) {
			ZSTR_VAL(result)[i] = flipper.bytes[sizeof(TValue) - i - 1];
		}

		ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';
	}

	return result;
}

template<size_t TYPE_BITS, typename TValue>
static inline zend_string* writeUnsignedVarInt(TValue value) {
	char result[VarIntConstants::MAX_BYTES<TYPE_BITS>];

	TValue remaining = value;

	for (auto i = 0; i < VarIntConstants::MAX_BYTES<TYPE_BITS>; i++) {
		unsigned char nextByte = remaining & VarIntConstants::VALUE_MASK;

		TValue nextRemaining = remaining >> VarIntConstants::BITS_PER_BYTE;

		if (nextRemaining == 0) {
			result[i] = nextByte;

			return zend_string_init(&result[0], i + 1, 0);
		}

		result[i] = nextByte | VarIntConstants::MSB_MASK;
		remaining = nextRemaining;
	}

	zend_value_error("Value too large to be encoded as a VarInt");
	return NULL;
}

template<size_t TYPE_BITS, typename TUnsignedType, typename TSignedType>
static inline zend_string* writeSignedVarInt(TSignedType value) {
	TUnsignedType mask = 0;
	if (value < 0) {
		//we don't know the type of TUnsignedType here, can't use ~0 directly (the compiler will optimise this anyway)
		mask = ~mask;
	}

	return writeUnsignedVarInt<TYPE_BITS, TUnsignedType>((static_cast<TUnsignedType>(value) << 1) ^ mask);
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
	ZEND_RAW_FENTRY(zend_name "LE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::LittleEndian>), result_wrapper>), arg_info, 0) \
	ZEND_RAW_FENTRY(zend_name "BE", (zif_readType<native_type, (readFixedSizeType<native_type, ByteOrder::BigEndian>), result_wrapper>), arg_info, 0)

#define READ_VARINT_FENTRY(size, size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("readUnsignedVar" size_name, (zif_readType<unsigned_type, (readUnsignedVarInt<size, unsigned_type>), zval_long_wrapper>), arginfo_read_integer, 0) \
	ZEND_RAW_FENTRY("readSignedVar" size_name, (zif_readType<signed_type, (readSignedVarInt<size, unsigned_type, signed_type>), zval_long_wrapper>), arginfo_read_integer, 0)

#define WRITE_FIXED_TYPE_FENTRY(zend_name, native_type, parse_parameters_wrapper, arg_info) \
	ZEND_RAW_FENTRY(zend_name "LE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::LittleEndian>)>), arg_info, 0) \
	ZEND_RAW_FENTRY(zend_name "BE", (zif_writeType<native_type, parse_parameters_wrapper<native_type>, (writeFixedSizeType<native_type, ByteOrder::BigEndian>)>), arg_info, 0)

#define WRITE_VARINT_FENTRY(size, size_name, unsigned_type, signed_type) \
	ZEND_RAW_FENTRY("writeUnsignedVar" size_name, (zif_writeType<unsigned_type, zend_parse_parameters_long_wrapper<unsigned_type>, (writeUnsignedVarInt<size, unsigned_type>)>), arginfo_write_integer, 0) \
	ZEND_RAW_FENTRY("writeSignedVar" size_name, (zif_writeType<signed_type, zend_parse_parameters_long_wrapper<signed_type>, (writeSignedVarInt<size, unsigned_type, signed_type>)>), arginfo_write_integer, 0)

zend_function_entry ext_functions[] = {
	READ_FIXED_TYPE_FENTRY("readUnsignedShort", uint16_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedShort", int16_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readUnsignedInt", uint32_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedInt", int32_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readSignedLong", uint64_t, zval_long_wrapper, arginfo_read_integer)
	READ_FIXED_TYPE_FENTRY("readFloat", float, zval_double_wrapper, arginfo_read_float)
	READ_FIXED_TYPE_FENTRY("readDouble", double, zval_double_wrapper, arginfo_read_float)

	READ_VARINT_FENTRY(32, "Int", uint32_t, int32_t)
	READ_VARINT_FENTRY(64, "Long", uint64_t, int64_t)

	WRITE_FIXED_TYPE_FENTRY("writeShort", int16_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeInt", int32_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeLong", int64_t, zend_parse_parameters_long_wrapper, arginfo_write_integer)
	WRITE_FIXED_TYPE_FENTRY("writeFloat", float, zend_parse_parameters_double_wrapper, arginfo_write_float)
	WRITE_FIXED_TYPE_FENTRY("writeDouble", double, zend_parse_parameters_double_wrapper, arginfo_write_float)

	WRITE_VARINT_FENTRY(32, "Int", uint32_t, int32_t)
	WRITE_VARINT_FENTRY(64, "Long", uint64_t, int64_t)

	PHP_FE_END
};

PHP_MINIT_FUNCTION(encoding) {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "DataDecodeException", NULL);
	data_decode_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);
	data_decode_exception_ce->ce_flags |= ZEND_ACC_FINAL;

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
	ext_functions,					/* zend_function_entry */
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
