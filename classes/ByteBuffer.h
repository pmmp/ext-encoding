#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

extern "C" {
#include "php.h"
}
#include "../ZendUtil.h"

typedef struct _byte_buffer_zend_object {
	zend_string* buffer;
	size_t read_offset;
	size_t write_offset;
	size_t used;
	zend_object std;
} byte_buffer_zend_object;

#define BYTE_BUFFER_FROM_ZVAL(zv) fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(zv))
#define BYTE_BUFFER_THIS() BYTE_BUFFER_FROM_ZVAL(ZEND_THIS)

extern zend_class_entry* byte_buffer_ce;

zend_class_entry* init_class_ByteBuffer(void);
#endif
