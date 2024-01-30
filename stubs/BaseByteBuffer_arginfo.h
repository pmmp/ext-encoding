/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7cd1dd363517d6bc01b82e50e5dc8e16c3ac569b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, buffer, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_readByteArray, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_writeByteArray, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_getReadOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_BaseByteBuffer_getWriteOffset arginfo_class_pmmp_encoding_BaseByteBuffer_getReadOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_setReadOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_BaseByteBuffer_setWriteOffset arginfo_class_pmmp_encoding_BaseByteBuffer_setReadOffset

#define arginfo_class_pmmp_encoding_BaseByteBuffer_getUsedLength arginfo_class_pmmp_encoding_BaseByteBuffer_getReadOffset

#define arginfo_class_pmmp_encoding_BaseByteBuffer_getReservedLength arginfo_class_pmmp_encoding_BaseByteBuffer_getReadOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_trim, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_BaseByteBuffer___debugInfo arginfo_class_pmmp_encoding_BaseByteBuffer___serialize


ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __construct);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, toString);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, readByteArray);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, writeByteArray);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getReadOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getWriteOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, setReadOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, setWriteOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getUsedLength);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getReservedLength);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, reserve);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, trim);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __serialize);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __unserialize);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __debugInfo);


static const zend_function_entry class_pmmp_encoding_BaseByteBuffer_methods[] = {
	ZEND_ME(pmmp_encoding_BaseByteBuffer, __construct, arginfo_class_pmmp_encoding_BaseByteBuffer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, toString, arginfo_class_pmmp_encoding_BaseByteBuffer_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, readByteArray, arginfo_class_pmmp_encoding_BaseByteBuffer_readByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, writeByteArray, arginfo_class_pmmp_encoding_BaseByteBuffer_writeByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getReadOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_getReadOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getWriteOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_getWriteOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, setReadOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_setReadOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, setWriteOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_setWriteOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getUsedLength, arginfo_class_pmmp_encoding_BaseByteBuffer_getUsedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getReservedLength, arginfo_class_pmmp_encoding_BaseByteBuffer_getReservedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, reserve, arginfo_class_pmmp_encoding_BaseByteBuffer_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, trim, arginfo_class_pmmp_encoding_BaseByteBuffer_trim, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, __serialize, arginfo_class_pmmp_encoding_BaseByteBuffer___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, __unserialize, arginfo_class_pmmp_encoding_BaseByteBuffer___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, __debugInfo, arginfo_class_pmmp_encoding_BaseByteBuffer___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_BaseByteBuffer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "BaseByteBuffer", class_pmmp_encoding_BaseByteBuffer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return class_entry;
}
