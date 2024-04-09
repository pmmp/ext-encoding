#ifndef BASE_BYTE_BUFFER_H
#define BASE_BYTE_BUFFER_H

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

#define BYTE_BUFFER_THIS() fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(ZEND_THIS))

zend_class_entry* init_class_BaseByteBuffer(void);
#endif
