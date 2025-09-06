/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7b065b4b7170dbe7ecb0a00efd18a8c72dba4815 */

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
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("__construct", zim_pmmp_encoding_ByteBufferWriter___construct, arginfo_class_pmmp_encoding_ByteBufferWriter___construct, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Constructs a new ByteBufferWriter.\n     * The provided string will be written at the start of the buffer as if readByteArray() was called.\n	 */")
#else
	ZEND_RAW_FENTRY("__construct", zim_pmmp_encoding_ByteBufferWriter___construct, arginfo_class_pmmp_encoding_ByteBufferWriter___construct, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getData", zim_pmmp_encoding_ByteBufferWriter_getData, arginfo_class_pmmp_encoding_ByteBufferWriter_getData, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns a string containing the written bytes.\n	 * Reserved memory is not included.\n	 */")
#else
	ZEND_RAW_FENTRY("getData", zim_pmmp_encoding_ByteBufferWriter_getData, arginfo_class_pmmp_encoding_ByteBufferWriter_getData, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("writeByteArray", zim_pmmp_encoding_ByteBufferWriter_writeByteArray, arginfo_class_pmmp_encoding_ByteBufferWriter_writeByteArray, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Writes the given bytes to the buffer at the current offset.\n	 * The internal offset will be updated by this operation.\n	 *\n	 * If the current buffer size is not big enough to add the given\n	 * bytes, the buffer will be resized to either 2x its current size,\n	 * or the actual size of the result, whichever is larger. This\n	 * ensures the lowest number of reallocations.\n	 */")
#else
	ZEND_RAW_FENTRY("writeByteArray", zim_pmmp_encoding_ByteBufferWriter_writeByteArray, arginfo_class_pmmp_encoding_ByteBufferWriter_writeByteArray, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getOffset", zim_pmmp_encoding_ByteBufferWriter_getOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns the current internal write offset (the position\n	 * from which the next write operation will start).\n	 */")
#else
	ZEND_RAW_FENTRY("getOffset", zim_pmmp_encoding_ByteBufferWriter_getOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_getOffset, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("setOffset", zim_pmmp_encoding_ByteBufferWriter_setOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_setOffset, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Sets the internal write offset to the given value.\n	 * The offset must be within the bounds of the buffer\n	 * (0 <= offset <= reserved length).\n	 *\n	 * @throws \\ValueError if the offset is out of bounds\n	 */")
#else
	ZEND_RAW_FENTRY("setOffset", zim_pmmp_encoding_ByteBufferWriter_setOffset, arginfo_class_pmmp_encoding_ByteBufferWriter_setOffset, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getUsedLength", zim_pmmp_encoding_ByteBufferWriter_getUsedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getUsedLength, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns the total number of bytes written.\n	 * This will always be less than or equal to the reserved length.\n	 */")
#else
	ZEND_RAW_FENTRY("getUsedLength", zim_pmmp_encoding_ByteBufferWriter_getUsedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getUsedLength, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("getReservedLength", zim_pmmp_encoding_ByteBufferWriter_getReservedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getReservedLength, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Returns the number of bytes reserved by the ByteBuffer.\n	 * This value may be larger than the number of written bytes, as\n	 * some memory may be preallocated to avoid reallocations.\n	 */")
#else
	ZEND_RAW_FENTRY("getReservedLength", zim_pmmp_encoding_ByteBufferWriter_getReservedLength, arginfo_class_pmmp_encoding_ByteBufferWriter_getReservedLength, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("reserve", zim_pmmp_encoding_ByteBufferWriter_reserve, arginfo_class_pmmp_encoding_ByteBufferWriter_reserve, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Increases buffer capacity to the given value, if the capacity\n	 * is less than this amount. Useful to avoid extra reallocations\n	 * during large write operations when the needed capacity of the\n	 * buffer is known in advance.\n	 */")
#else
	ZEND_RAW_FENTRY("reserve", zim_pmmp_encoding_ByteBufferWriter_reserve, arginfo_class_pmmp_encoding_ByteBufferWriter_reserve, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("trim", zim_pmmp_encoding_ByteBufferWriter_trim, arginfo_class_pmmp_encoding_ByteBufferWriter_trim, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Truncates the internal buffer to only the written part,\n	 * discarding any unused reserved memory.\n	 */")
#else
	ZEND_RAW_FENTRY("trim", zim_pmmp_encoding_ByteBufferWriter_trim, arginfo_class_pmmp_encoding_ByteBufferWriter_trim, ZEND_ACC_PUBLIC)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("clear", zim_pmmp_encoding_ByteBufferWriter_clear, arginfo_class_pmmp_encoding_ByteBufferWriter_clear, ZEND_ACC_PUBLIC, NULL, "/**\n	 * Clears all data from the buffer. The memory used is retained\n	 * as reserved memory.\n	 */")
#else
	ZEND_RAW_FENTRY("clear", zim_pmmp_encoding_ByteBufferWriter_clear, arginfo_class_pmmp_encoding_ByteBufferWriter_clear, ZEND_ACC_PUBLIC)
#endif
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
