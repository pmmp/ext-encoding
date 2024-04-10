/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 271225664feb2879c5d4ade51ca57673fee2af99 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, buffer, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_readByteArray, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_writeByteArray, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_getReadOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBuffer_getWriteOffset arginfo_class_pmmp_encoding_ByteBuffer_getReadOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_setReadOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBuffer_setWriteOffset arginfo_class_pmmp_encoding_ByteBuffer_setReadOffset

#define arginfo_class_pmmp_encoding_ByteBuffer_getUsedLength arginfo_class_pmmp_encoding_ByteBuffer_getReadOffset

#define arginfo_class_pmmp_encoding_ByteBuffer_getReservedLength arginfo_class_pmmp_encoding_ByteBuffer_getReadOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer_trim, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBuffer_clear arginfo_class_pmmp_encoding_ByteBuffer_trim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_ByteBuffer___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_ByteBuffer___debugInfo arginfo_class_pmmp_encoding_ByteBuffer___serialize


ZEND_METHOD(pmmp_encoding_ByteBuffer, __construct);
ZEND_METHOD(pmmp_encoding_ByteBuffer, toString);
ZEND_METHOD(pmmp_encoding_ByteBuffer, readByteArray);
ZEND_METHOD(pmmp_encoding_ByteBuffer, writeByteArray);
ZEND_METHOD(pmmp_encoding_ByteBuffer, getReadOffset);
ZEND_METHOD(pmmp_encoding_ByteBuffer, getWriteOffset);
ZEND_METHOD(pmmp_encoding_ByteBuffer, setReadOffset);
ZEND_METHOD(pmmp_encoding_ByteBuffer, setWriteOffset);
ZEND_METHOD(pmmp_encoding_ByteBuffer, getUsedLength);
ZEND_METHOD(pmmp_encoding_ByteBuffer, getReservedLength);
ZEND_METHOD(pmmp_encoding_ByteBuffer, reserve);
ZEND_METHOD(pmmp_encoding_ByteBuffer, trim);
ZEND_METHOD(pmmp_encoding_ByteBuffer, clear);
ZEND_METHOD(pmmp_encoding_ByteBuffer, __serialize);
ZEND_METHOD(pmmp_encoding_ByteBuffer, __unserialize);
ZEND_METHOD(pmmp_encoding_ByteBuffer, __debugInfo);


static const zend_function_entry class_pmmp_encoding_ByteBuffer_methods[] = {
	ZEND_ME(pmmp_encoding_ByteBuffer, __construct, arginfo_class_pmmp_encoding_ByteBuffer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, toString, arginfo_class_pmmp_encoding_ByteBuffer_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, readByteArray, arginfo_class_pmmp_encoding_ByteBuffer_readByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, writeByteArray, arginfo_class_pmmp_encoding_ByteBuffer_writeByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, getReadOffset, arginfo_class_pmmp_encoding_ByteBuffer_getReadOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, getWriteOffset, arginfo_class_pmmp_encoding_ByteBuffer_getWriteOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, setReadOffset, arginfo_class_pmmp_encoding_ByteBuffer_setReadOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, setWriteOffset, arginfo_class_pmmp_encoding_ByteBuffer_setWriteOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, getUsedLength, arginfo_class_pmmp_encoding_ByteBuffer_getUsedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, getReservedLength, arginfo_class_pmmp_encoding_ByteBuffer_getReservedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, reserve, arginfo_class_pmmp_encoding_ByteBuffer_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, trim, arginfo_class_pmmp_encoding_ByteBuffer_trim, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, clear, arginfo_class_pmmp_encoding_ByteBuffer_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, __serialize, arginfo_class_pmmp_encoding_ByteBuffer___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, __unserialize, arginfo_class_pmmp_encoding_ByteBuffer___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_ByteBuffer, __debugInfo, arginfo_class_pmmp_encoding_ByteBuffer___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_ByteBuffer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "ByteBuffer", class_pmmp_encoding_ByteBuffer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return class_entry;
}
