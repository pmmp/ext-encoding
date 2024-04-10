extern "C" {
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "../stubs/BaseByteBuffer_arginfo.h"
}

#include "BaseByteBuffer.h"
#include "DataDecodeException.h"
#include "../Serializers.h"

static zend_object_handlers byte_buffer_zend_object_handlers;
static zend_class_entry* base_byte_buffer_ce;

static void byte_buffer_init_properties(byte_buffer_zend_object* object, zend_string* buffer, size_t used, size_t read_offset, size_t write_offset) {
	object->buffer = buffer;
	zend_string_addref(buffer);
	object->read_offset = read_offset;
	object->write_offset = write_offset;
	object->used = used;
}

static zend_object* byte_buffer_new(zend_class_entry* ce) {
	auto object = alloc_custom_zend_object<byte_buffer_zend_object>(ce, &byte_buffer_zend_object_handlers);

	byte_buffer_init_properties(object, zend_empty_string, 0, 0, 0);

	return &object->std;
}

static zend_object* byte_buffer_clone(zend_object* object) {
	auto old_object = fetch_from_zend_object<byte_buffer_zend_object>(object);
	auto new_object = fetch_from_zend_object<byte_buffer_zend_object>(byte_buffer_new(object->ce));

	zend_objects_clone_members(&new_object->std, &old_object->std);

	byte_buffer_init_properties(new_object, old_object->buffer, old_object->used, old_object->read_offset, old_object->write_offset);

	return &new_object->std;
}

static void byte_buffer_free(zend_object* std) {
	auto object = fetch_from_zend_object<byte_buffer_zend_object>(std);

	zend_string_release_ex(object->buffer, 0);
}

static int byte_buffer_compare_objects(zval* obj1, zval* obj2) {
	if (Z_TYPE_P(obj1) == IS_OBJECT && Z_TYPE_P(obj2) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE_P(obj1), base_byte_buffer_ce) && instanceof_function(Z_OBJCE_P(obj2), base_byte_buffer_ce)) {
			auto object1 = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(obj1));
			auto object2 = fetch_from_zend_object<byte_buffer_zend_object>(Z_OBJ_P(obj2));

			if (
				object1->read_offset == object2->read_offset &&
				object1->write_offset == object2->write_offset &&
				object1->used == object2->used &&
				zend_string_equals(object1->buffer, object2->buffer)
				) {
				return 0;
			}
		}
	}

	return 1;
}

#define BYTE_BUFFER_METHOD(name) PHP_METHOD(pmmp_encoding_BaseByteBuffer, name)

BYTE_BUFFER_METHOD(__construct) {
	zend_string* buffer = NULL;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(buffer)
		ZEND_PARSE_PARAMETERS_END();

	object = BYTE_BUFFER_THIS();
	if (object->buffer) {
		zend_string_release_ex(object->buffer, 0);
	}

	if (buffer == NULL) {
		buffer = zend_empty_string;
	}
	//read offset is placed at the start, and write offset at the end (to mirror PM BinaryStream behaviour)
	byte_buffer_init_properties(object, buffer, ZSTR_LEN(buffer), 0, ZSTR_LEN(buffer));
}

BYTE_BUFFER_METHOD(toString) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_STRINGL(ZSTR_VAL(object->buffer), object->used);
}

BYTE_BUFFER_METHOD(readByteArray) {
	zend_long zlength;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
		ZEND_PARSE_PARAMETERS_END();

	if (zlength < 0) {
		zend_value_error("Length cannot be negative");
		return;
	}
	if (zlength == 0) { //to mirror PM BinaryStream behaviour
		RETURN_STR(zend_empty_string);
	}

	size_t length = static_cast<size_t>(zlength);

	object = BYTE_BUFFER_THIS();

	if (object->used - object->read_offset < length) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", length, object->used - object->read_offset);
		return;
	}

	RETVAL_STRINGL(ZSTR_VAL(object->buffer) + object->read_offset, length);
	object->read_offset += length;
}

BYTE_BUFFER_METHOD(writeByteArray) {
	zend_string* value;
	byte_buffer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(value)
		ZEND_PARSE_PARAMETERS_END();


	object = BYTE_BUFFER_THIS();

	auto size = ZSTR_LEN(value);

	extendBuffer(object->buffer, object->write_offset, size);
	memcpy(ZSTR_VAL(object->buffer) + object->write_offset, ZSTR_VAL(value), size);
	object->write_offset += size;
	if (object->write_offset > object->used) {
		object->used = object->write_offset;
	}
}

#define OFFSET_METHODS(func_name, which_offset) \
	BYTE_BUFFER_METHOD(get##func_name) { \
		zend_parse_parameters_none_throw(); \
		auto object = BYTE_BUFFER_THIS(); \
		RETURN_LONG(object->which_offset); \
	} \
	BYTE_BUFFER_METHOD(set##func_name) { \
		zend_long offset; \
	\
		ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1) \
			Z_PARAM_LONG(offset) \
		ZEND_PARSE_PARAMETERS_END(); \
	\
		auto object = BYTE_BUFFER_THIS(); \
		if (offset < 0 || static_cast<size_t>(offset) > object->used) { \
			zend_value_error("Offset must not be less than zero or greater than the buffer size"); \
			return; \
		} \
	\
		object->which_offset = static_cast<size_t>(offset); \
	}

OFFSET_METHODS(ReadOffset, read_offset);
OFFSET_METHODS(WriteOffset, write_offset);

BYTE_BUFFER_METHOD(getUsedLength) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_LONG(object->used);
}

BYTE_BUFFER_METHOD(getReservedLength) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	RETURN_LONG(ZSTR_LEN(object->buffer));
}

BYTE_BUFFER_METHOD(reserve) {
	zend_long zlength;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
		ZEND_PARSE_PARAMETERS_END();

	if (zlength <= 0) {
		zend_value_error("Length must be greater than zero");
		return;
	}
	auto object = BYTE_BUFFER_THIS();
	extendBuffer(object->buffer, static_cast<size_t>(zlength), 0);
}

BYTE_BUFFER_METHOD(trim) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	if (ZSTR_LEN(object->buffer) > object->used) {
		object->buffer = zend_string_truncate(object->buffer, object->used, 0);
	}
}

BYTE_BUFFER_METHOD(clear) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	object->read_offset = 0;
	object->write_offset = 0;
	object->used = 0;
}

BYTE_BUFFER_METHOD(__serialize) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	array_init(return_value);
	add_assoc_stringl(return_value, "buffer", ZSTR_VAL(object->buffer), object->used);
	add_assoc_long(return_value, "read_offset", object->read_offset);
	add_assoc_long(return_value, "write_offset", object->write_offset);
}

static zval* fetch_serialized_property(HashTable* data, const char* name, int type) {
	zval* zv = zend_hash_str_find(data, name, strlen(name));
	if (zv == NULL) {
		zend_throw_exception_ex(NULL, 0, "Serialized data is missing \"%s\"", name);
		return NULL;
	}
	if (Z_TYPE_P(zv) != type) {
		zend_throw_exception_ex(NULL, 0, "\"%s\" in serialized data should be of type %s, but have %s", name, zend_zval_type_name(zv), zend_get_type_by_const(type));
		return NULL;
	}

	return zv;
}

BYTE_BUFFER_METHOD(__unserialize) {
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
		ZEND_PARSE_PARAMETERS_END();

	zval* buffer = fetch_serialized_property(data, "buffer", IS_STRING);
	if (buffer == NULL) {
		return;
	}
	zval* read_offset = fetch_serialized_property(data, "read_offset", IS_LONG);
	if (read_offset == NULL) {
		return;
	}
	zval* write_offset = fetch_serialized_property(data, "write_offset", IS_LONG);
	if (write_offset == NULL) {
		return;
	}

	auto object = BYTE_BUFFER_THIS();

	byte_buffer_init_properties(object, Z_STR_P(buffer), Z_STRLEN_P(buffer), Z_LVAL_P(read_offset), Z_LVAL_P(write_offset));
}

BYTE_BUFFER_METHOD(__debugInfo) {
	zend_parse_parameters_none_throw();

	auto object = BYTE_BUFFER_THIS();
	array_init(return_value);
	add_assoc_stringl(return_value, "buffer", ZSTR_VAL(object->buffer), object->used);
	add_assoc_long(return_value, "read_offset", object->read_offset);
	add_assoc_long(return_value, "write_offset", object->write_offset);
}

zend_class_entry* init_class_BaseByteBuffer(void) {
	base_byte_buffer_ce = register_class_pmmp_encoding_BaseByteBuffer();
	base_byte_buffer_ce->create_object = byte_buffer_new;

	byte_buffer_zend_object_handlers = *zend_get_std_object_handlers();
	byte_buffer_zend_object_handlers.offset = XtOffsetOf(byte_buffer_zend_object, std);
	byte_buffer_zend_object_handlers.clone_obj = byte_buffer_clone;
	byte_buffer_zend_object_handlers.free_obj = byte_buffer_free;
	byte_buffer_zend_object_handlers.compare = byte_buffer_compare_objects;

	return base_byte_buffer_ce;
}
