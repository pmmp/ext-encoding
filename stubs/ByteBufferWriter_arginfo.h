/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f10e34840268133620b2fd4940a64247fe8816ac */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_getData, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_writeByteArray, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_setOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBufferWriter_getUsedLength arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset

#define arginfo_class_pmmp_encoding_ByteBufferWriter_getReservedLength arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter_trim, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBufferWriter_clear arginfo_class_pmmp_encoding_ByteBufferWriter_trim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferWriter___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBufferWriter___debugInfo arginfo_class_pmmp_encoding_ByteBufferWriter___serialize

ZEND_METHOD(pmmp_encoding_ByteBufferWriter, __construct);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, getData);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, writeByteArray);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, getOffset);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, setOffset);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, getUsedLength);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, getReservedLength);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, reserve);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, trim);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, clear);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, __serialize);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, __unserialize);
ZEND_METHOD(pmmp_encoding_ByteBufferWriter, __debugInfo);

static const zend_function_entry class_pmmp_encoding_ByteBufferWriter_methods[] = {
	ZEND_ME(pmmp_encoding_ByteBufferWriter, __construct, arginfo_class_pmmp_encoding_ByteBufferWriter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, getData, arginfo_class_pmmp_encoding_ByteBufferWriter_getData, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, writeByteArray, arginfo_class_pmmp_encoding_ByteBufferWriter_writeByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, getOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, setOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_setOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, getUsedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getUsedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, getReservedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getReservedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, reserve, arginfo_class_pmmp_encoding_ByteBufferWriter_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, trim, arginfo_class_pmmp_encoding_ByteBufferWriter_trim, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, clear, arginfo_class_pmmp_encoding_ByteBufferWriter_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, __serialize, arginfo_class_pmmp_encoding_ByteBufferWriter___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, __unserialize, arginfo_class_pmmp_encoding_ByteBufferWriter___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferWriter, __debugInfo, arginfo_class_pmmp_encoding_ByteBufferWriter___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_ByteBufferWriter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "ByteBufferWriter", class_pmmp_encoding_ByteBufferWriter_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}
