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
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
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
			zend_value_error("$offset expects at least 0, %zu given", offset);
			return false;
		}
	}

	return true;
}

static inline void setOffsetReferenceParameter(zval* const zoffset, const zend_long offset) {
	if (zoffset != NULL) {
		ZVAL_LONG(Z_REFVAL_P(zoffset), offset);
	}
}

template<typename TValue, ByteOrder byteOrder, void(*assignResult)(zval*, TValue)>
void ZEND_FASTCALL zif_readFixedSizeType(INTERNAL_FUNCTION_PARAMETERS) {
	zend_string* bytes;
	zval* zoffset = NULL;
	zend_long offset = 0;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_STR(bytes)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_EX(zoffset, 0, 0)
	ZEND_PARSE_PARAMETERS_END();

	if (!handleOffsetReferenceParameter(zoffset, offset)) {
		return;
	}

	const auto SIZE = sizeof(TValue);
	if (ZSTR_LEN(bytes) - offset < SIZE) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, ZSTR_LEN(bytes));
		return;
	}

	setOffsetReferenceParameter(zoffset, offset + SIZE);

	if (byteOrder == ByteOrder::Native) {
		assignResult(return_value, *(reinterpret_cast<TValue*>(&ZSTR_VAL(bytes)[offset])));

		return;
	}

	//endian flip
	union {
		char bytes[sizeof(TValue)];
		TValue value;
	} result;

	for (auto i = 0; i < sizeof(TValue); i++) {
		result.bytes[sizeof(TValue) - i - 1] = ZSTR_VAL(bytes)[i + offset];
	}
	assignResult(return_value, result.value);
}

template<bool (*readVarIntFunc)(zend_string* bytes, zend_long& offset, zend_ulong& result)>
void ZEND_FASTCALL zif_readVarInt(INTERNAL_FUNCTION_PARAMETERS) {
	zend_string* bytes;
	zval* zoffset = NULL;
	zend_long offset = 0;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
		Z_PARAM_STR(bytes)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_EX(zoffset, 0, 0)
	ZEND_PARSE_PARAMETERS_END();

	if (!handleOffsetReferenceParameter(zoffset, offset)) {
		return;
	}

	zend_ulong result;
	if (readVarIntFunc(bytes, offset, result)) {
		setOffsetReferenceParameter(zoffset, offset);
		RETURN_LONG((zend_long) result);
	}
}

template<size_t TYPE_BITS>
static inline bool readUnsignedVarInt(zend_string *bytes, zend_long &offset, zend_ulong &result) {

	const auto MAX_BYTES = TYPE_BITS / 7 + (TYPE_BITS % 7 > 0);
	const auto VARINT_VALUE_MASK = 0x7f;
	const auto VARINT_MSB_MASK = 0x80;

	result = 0;
	for (auto shift = 0; shift < TYPE_BITS; shift += 7) {
		if (offset >= ZSTR_LEN(bytes)) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "No bytes left in buffer", 0);
			return false;
		}
		zend_ulong byte = (zend_ulong) ZSTR_VAL(bytes)[offset++];

		result |= ((byte & VARINT_VALUE_MASK) << shift);
		if ((byte & VARINT_MSB_MASK) == 0) {
			return true;
		}
	}

	zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "VarInt did not terminate after %u bytes!", MAX_BYTES);
	return false;
}

template<size_t TYPE_BITS>
static inline bool readSignedVarInt(zend_string* bytes, zend_long& offset, zend_ulong& result) {
	if (!readUnsignedVarInt<TYPE_BITS>(bytes, offset, result)) {
		return false;
	}

	zend_ulong mask = result & 1 ? ~0ULL : 0ULL;

	result = (result >> 1) ^ mask;
	return true;
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

#define DEFINE_FENTRY(zend_name, native_type, result_wrapper, arg_info) \
	ZEND_RAW_FENTRY(zend_name "LE", (zif_readFixedSizeType<native_type, ByteOrder::LittleEndian, result_wrapper>), arg_info, 0) \
	ZEND_RAW_FENTRY(zend_name "BE", (zif_readFixedSizeType<native_type, ByteOrder::BigEndian, result_wrapper>), arg_info, 0)

zend_function_entry ext_functions[] = {
	DEFINE_FENTRY("readUnsignedShort", uint16_t, zval_long_wrapper, arginfo_read_integer)
	DEFINE_FENTRY("readShort", int16_t, zval_long_wrapper, arginfo_read_integer)
	DEFINE_FENTRY("readUnsignedInt", uint32_t, zval_long_wrapper, arginfo_read_integer)
	DEFINE_FENTRY("readInt", int32_t, zval_long_wrapper, arginfo_read_integer)
	DEFINE_FENTRY("readLong", uint64_t, zval_long_wrapper, arginfo_read_integer)
	DEFINE_FENTRY("readFloat", float, zval_double_wrapper, arginfo_read_float)
	DEFINE_FENTRY("readDouble", double, zval_double_wrapper, arginfo_read_float)
	ZEND_RAW_FENTRY("readUnsignedVarInt", zif_readVarInt<readUnsignedVarInt<32>>, arginfo_read_integer, 0)
	ZEND_RAW_FENTRY("readUnsignedVarLong", zif_readVarInt<readUnsignedVarInt<64>>, arginfo_read_integer, 0)
	ZEND_RAW_FENTRY("readSignedVarInt", zif_readVarInt<readSignedVarInt<32>>, arginfo_read_integer, 0)
	ZEND_RAW_FENTRY("readSignedVarLong", zif_readVarInt<readSignedVarInt<64>>, arginfo_read_integer, 0)
	PHP_FE_END
};

/* {{{ encoding_module_entry */
zend_module_entry encoding_module_entry = {
	STANDARD_MODULE_HEADER,
	"encoding",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
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
