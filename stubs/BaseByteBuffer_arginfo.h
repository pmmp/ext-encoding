/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3bf9e52957f8e887b5addfdd7cc753b098a141f1 */

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_getOffset, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_setOffset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_BaseByteBuffer_getReserved arginfo_class_pmmp_encoding_BaseByteBuffer_getOffset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_pmmp_encoding_BaseByteBuffer_trim, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_pmmp_encoding_BaseByteBuffer_rewind arginfo_class_pmmp_encoding_BaseByteBuffer_trim

#define arginfo_class_pmmp_encoding_BaseByteBuffer_getUnreadLength arginfo_class_pmmp_encoding_BaseByteBuffer_getOffset

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
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, setOffset);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getReserved);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, reserve);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, trim);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, rewind);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, getUnreadLength);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __serialize);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __unserialize);
ZEND_METHOD(pmmp_encoding_BaseByteBuffer, __debugInfo);


static const zend_function_entry class_pmmp_encoding_BaseByteBuffer_methods[] = {
	ZEND_ME(pmmp_encoding_BaseByteBuffer, __construct, arginfo_class_pmmp_encoding_BaseByteBuffer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, toString, arginfo_class_pmmp_encoding_BaseByteBuffer_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, readByteArray, arginfo_class_pmmp_encoding_BaseByteBuffer_readByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, writeByteArray, arginfo_class_pmmp_encoding_BaseByteBuffer_writeByteArray, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, setOffset, arginfo_class_pmmp_encoding_BaseByteBuffer_setOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getReserved, arginfo_class_pmmp_encoding_BaseByteBuffer_getReserved, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, reserve, arginfo_class_pmmp_encoding_BaseByteBuffer_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, trim, arginfo_class_pmmp_encoding_BaseByteBuffer_trim, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, rewind, arginfo_class_pmmp_encoding_BaseByteBuffer_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(pmmp_encoding_BaseByteBuffer, getUnreadLength, arginfo_class_pmmp_encoding_BaseByteBuffer_getUnreadLength, ZEND_ACC_PUBLIC)
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
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;

	return class_entry;
}