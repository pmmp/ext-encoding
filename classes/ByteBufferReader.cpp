extern "C" {
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "../stubs/ByteBufferReader_arginfo.h"
}

#include "ByteBufferReader.h"
#include "DataDecodeException.h"
#include "../Serializers.h"

static zend_object_handlers byte_buffer_reader_zend_object_handlers;
zend_class_entry* byte_buffer_reader_ce;

static void reader_init_properties(byte_buffer_reader_zend_object* object, zend_string* buffer, size_t offset) {
	object->buffer = buffer;
	zend_string_addref(buffer);
	object->offset = offset;
}

static zend_object* reader_new(zend_class_entry* ce) {
	auto object = alloc_custom_zend_object<byte_buffer_reader_zend_object>(ce, &byte_buffer_reader_zend_object_handlers);

	reader_init_properties(object, zend_empty_string, 0);

	return &object->std;
}

static zend_object* reader_clone(zend_object* object) {
	auto old_object = fetch_from_zend_object<byte_buffer_reader_zend_object>(object);
	auto new_object = fetch_from_zend_object<byte_buffer_reader_zend_object>(reader_new(object->ce));

	zend_objects_clone_members(&new_object->std, &old_object->std);

	reader_init_properties(new_object, old_object->buffer, old_object->offset);

	return &new_object->std;
}

static void reader_free(zend_object* std) {
	auto object = fetch_from_zend_object<byte_buffer_reader_zend_object>(std);

	zend_string_release_ex(object->buffer, 0);
}

static int reader_compare_objects(zval* obj1, zval* obj2) {
	if (Z_TYPE_P(obj1) == IS_OBJECT && Z_TYPE_P(obj2) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE_P(obj1), byte_buffer_reader_ce) && instanceof_function(Z_OBJCE_P(obj2), byte_buffer_reader_ce)) {
			auto object1 = fetch_from_zend_object<byte_buffer_reader_zend_object>(Z_OBJ_P(obj1));
			auto object2 = fetch_from_zend_object<byte_buffer_reader_zend_object>(Z_OBJ_P(obj2));

			if (
				object1->offset == object2->offset &&
				zend_string_equals(object1->buffer, object2->buffer)
				) {
				return 0;
			}
		}
	}

	return 1;
}

#define READER_METHOD(name) PHP_METHOD(pmmp_encoding_ByteBufferReader, name)

READER_METHOD(__construct) {
	zend_string* buffer = NULL;
	byte_buffer_reader_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(buffer)
	ZEND_PARSE_PARAMETERS_END();

	object = READER_THIS();
	if (object->buffer) {
		zend_string_release_ex(object->buffer, 0);
	}

	reader_init_properties(object, buffer, 0);
}

READER_METHOD(getData) {
	zend_parse_parameters_none_throw();

	auto object = READER_THIS();
	RETURN_STR(object->buffer);
}

READER_METHOD(readByteArray) {
	zend_long zlength;
	byte_buffer_reader_zend_object* object;

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

	object = READER_THIS();

	if (ZSTR_LEN(object->buffer) - object->offset < length) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", length, ZSTR_LEN(object->buffer) - object->offset);
		return;
	}

	RETVAL_STRINGL(ZSTR_VAL(object->buffer) + object->offset, length);
	object->offset += length;
}

READER_METHOD(getOffset) {
	zend_parse_parameters_none_throw();
	auto object = READER_THIS();
	RETURN_LONG(object->offset);
}

READER_METHOD(setOffset) {
	zend_long offset;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	auto object = READER_THIS();
	if (offset < 0 || static_cast<size_t>(offset) > ZSTR_LEN(object->buffer)) {
		zend_value_error("Offset must not be less than zero or greater than the available data size");
		return;
	}

	object->offset = static_cast<size_t>(offset);
}

READER_METHOD(__serialize) {
	zend_parse_parameters_none_throw();

	auto object = READER_THIS();
	array_init(return_value);
	add_assoc_str(return_value, "buffer", object->buffer);
	add_assoc_long(return_value, "offset", object->offset);
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

READER_METHOD(__unserialize) {
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zval* buffer = fetch_serialized_property(data, "buffer", IS_STRING);
	if (buffer == NULL) {
		return;
	}
	zval* offset = fetch_serialized_property(data, "offset", IS_LONG);
	if (offset == NULL) {
		return;
	}

	auto object = READER_THIS();

	reader_init_properties(object, Z_STR_P(buffer), Z_LVAL_P(offset));
}

READER_METHOD(__debugInfo) {
	zend_parse_parameters_none_throw();

	auto object = READER_THIS();
	array_init(return_value);
	add_assoc_str(return_value, "buffer", object->buffer);
	add_assoc_long(return_value, "offset", object->offset);
}

zend_class_entry* init_class_ByteBufferReader(void) {
	byte_buffer_reader_ce = register_class_pmmp_encoding_ByteBufferReader();
	byte_buffer_reader_ce->create_object = reader_new;

	byte_buffer_reader_zend_object_handlers = *zend_get_std_object_handlers();
	byte_buffer_reader_zend_object_handlers.offset = XtOffsetOf(byte_buffer_reader_zend_object, std);
	byte_buffer_reader_zend_object_handlers.clone_obj = reader_clone;
	byte_buffer_reader_zend_object_handlers.free_obj = reader_free;
	byte_buffer_reader_zend_object_handlers.compare = reader_compare_objects;

	return byte_buffer_reader_ce;
}
