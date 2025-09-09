#ifndef BYTE_BUFFER_READER_H
#define BYTE_BUFFER_READER_H

extern "C" {
#include "php.h"
}
#include "../ZendUtil.h"

typedef struct _byte_buffer_reader_t {
	zend_string* buffer;
	size_t offset;
} byte_buffer_reader_t;

typedef struct _byte_buffer_reader_zend_object {
	byte_buffer_reader_t reader;
	zend_object std;
} byte_buffer_reader_zend_object;

#define READER_FROM_ZVAL(zv) fetch_from_zend_object<byte_buffer_reader_zend_object>(Z_OBJ_P(zv))
#define READER_THIS() READER_FROM_ZVAL(ZEND_THIS)

extern zend_class_entry* byte_buffer_reader_ce;

zend_class_entry* init_class_ByteBufferReader(void);
#endif
