#ifndef BYTE_BUFFER_WRITER_H
#define BYTE_BUFFER_WRITER_H

extern "C" {
#include "php.h"
}
#include "../ZendUtil.h"

typedef struct _byte_buffer_writer_t {
	zend_string* buffer;
	size_t offset;
	size_t used;
} byte_buffer_writer_t;

typedef struct _byte_buffer_writer_zend_object {
	byte_buffer_writer_t writer;
	zend_object std;
} byte_buffer_writer_zend_object;

#define WRITER_FROM_ZVAL(zv) fetch_from_zend_object<byte_buffer_writer_zend_object>(Z_OBJ_P(zv))
#define WRITER_THIS() WRITER_FROM_ZVAL(ZEND_THIS)

extern zend_class_entry* byte_buffer_writer_ce;

zend_class_entry* init_class_ByteBufferWriter(void);
#endif
