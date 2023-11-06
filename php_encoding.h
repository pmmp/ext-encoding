/* encoding extension for PHP */

#ifndef PHP_ENCODING_H
# define PHP_ENCODING_H

extern zend_module_entry encoding_module_entry;
# define phpext_encoding_ptr &encoding_module_entry

# define PHP_ENCODING_VERSION "0.2.2"

# if defined(ZTS) && defined(COMPILE_DL_ENCODING)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_ENCODING_H */
