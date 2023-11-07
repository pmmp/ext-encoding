/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2578db8a1c0b1ce61531799f4713e121b8907085 */




static const zend_function_entry class_pmmp_encoding_DataDecodeException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_pmmp_encoding_DataDecodeException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "pmmp\\encoding", "DataDecodeException", class_pmmp_encoding_DataDecodeException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
