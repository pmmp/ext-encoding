/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3989b5446234d41b6a97105428894d1f8429fde5 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader_getData, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader_readByteArray, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader_getOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader_setOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBufferReader___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBufferReader___debugInfo arginfo_class_pmmp_encoding_ByteBufferReader___serialize

ZEND_METHOD(pmmp_encoding_ByteBufferReader, __construct);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, getData);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, readByteArray);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, getOffset);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, setOffset);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, __serialize);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, __unserialize);
ZEND_METHOD(pmmp_encoding_ByteBufferReader, __debugInfo);

static const zend_function_entry class_pmmp_encoding_ByteBufferReader_methods[] = {
	ZEND_ME(pmmp_encoding_ByteBufferReader, __construct, arginfo_class_pmmp_encoding_ByteBufferReader___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, getData, arginfo_class_pmmp_encoding_ByteBufferReader_getData, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, readByteArray, arginfo_class_pmmp_encoding_ByteBufferReader_readByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, getOffset, arginfo_class_pmmp_encoding_ByteBufferReader_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, setOffset, arginfo_class_pmmp_encoding_ByteBufferReader_setOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, __serialize, arginfo_class_pmmp_encoding_ByteBufferReader___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, __unserialize, arginfo_class_pmmp_encoding_ByteBufferReader___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, __debugInfo, arginfo_class_pmmp_encoding_ByteBufferReader___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_ByteBufferReader(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "ByteBufferReader", class_pmmp_encoding_ByteBufferReader_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}
