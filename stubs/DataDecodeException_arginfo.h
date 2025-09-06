/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d9c3dfd4ba78bcdcd5e4af033c6f978a5b1265fc */

static zend_class_entry *register_class_pmmp_encoding_DataDecodeException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "DataDecodeException", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	return class_entry;
}
