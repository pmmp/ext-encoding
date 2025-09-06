/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: cf60f8c040fc3c12584760bd1db045b054100c6b */

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
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("__construct", zim_pmmp_encoding_ByteBufferReader___construct, arginfo_class_pmmp_encoding_ByteBufferReader___construct, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Constructs a new ByteBufferReader.\n	 * Offset will be initialized to 0.\n	 */")
#else
	ZEND_RAW_FENTRY("__construct", zim_pmmp_encoding_ByteBufferReader___construct, arginfo_class_pmmp_encoding_ByteBufferReader___construct, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getData", zim_pmmp_encoding_ByteBufferReader_getData, arginfo_class_pmmp_encoding_ByteBufferReader_getData, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns the string (byte array) that the reader is reading.\n	 */")
#else
	ZEND_RAW_FENTRY("getData", zim_pmmp_encoding_ByteBufferReader_getData, arginfo_class_pmmp_encoding_ByteBufferReader_getData, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("readByteArray", zim_pmmp_encoding_ByteBufferReader_readByteArray, arginfo_class_pmmp_encoding_ByteBufferReader_readByteArray, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Reads $length raw bytes from the buffer at the current offset.\n	 * The internal offset will be updated by this operation.\n     *\n     * @throws DataDecodeException if there are not enough bytes available\n	 */")
#else
	ZEND_RAW_FENTRY("readByteArray", zim_pmmp_encoding_ByteBufferReader_readByteArray, arginfo_class_pmmp_encoding_ByteBufferReader_readByteArray, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getOffset", zim_pmmp_encoding_ByteBufferReader_getOffset, arginfo_class_pmmp_encoding_ByteBufferReader_getOffset, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns the current internal read offset (the position\n	 * from which the next read operation will start).\n	 */")
#else
	ZEND_RAW_FENTRY("getOffset", zim_pmmp_encoding_ByteBufferReader_getOffset, arginfo_class_pmmp_encoding_ByteBufferReader_getOffset, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("setOffset", zim_pmmp_encoding_ByteBufferReader_setOffset, arginfo_class_pmmp_encoding_ByteBufferReader_setOffset, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Sets the internal read offset to the given value.\n	 * The offset must be within the bounds of the buffer\n	 * (0 <= offset <= used length).\n	 *\n	 * @throws \\ValueError if the offset is out of bounds\n	 */")
#else
	ZEND_RAW_FENTRY("setOffset", zim_pmmp_encoding_ByteBufferReader_setOffset, arginfo_class_pmmp_encoding_ByteBufferReader_setOffset, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(pmmp_encoding_ByteBufferReader, __serialize, arginfo_class_pmmp_encoding_ByteBufferReader___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, __unserialize, arginfo_class_pmmp_encoding_ByteBufferReader___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBufferReader, __debugInfo, arginfo_class_pmmp_encoding_ByteBufferReader___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_ByteBufferReader(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "ByteBufferReader", class_pmmp_encoding_ByteBufferReader_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}
