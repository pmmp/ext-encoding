extern "C" {
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "../stubs/ByteBufferWriter_arginfo.h"
}

#include "ByteBufferWriter.h"
#include "DataDecodeException.h"
#include "../Serializers.h"

static zend_object_handlers byte_buffer_writer_zend_object_handlers;
zend_class_entry* byte_buffer_writer_ce;

static void writer_init_properties(byte_buffer_writer_zend_object* object, unsigned char* buffer, size_t length, size_t offset) {
	object->writer.length = length; //we don't need to copy reserved memory
	object->writer.offset = offset;
	object->writer.used = length;
	if (length == 0) {
		const unsigned int preallocSize = 16;
		object->writer.buffer = reinterpret_cast<unsigned char*>(emalloc(preallocSize));
		object->writer.length = preallocSize;
	} else {
		object->writer.buffer = reinterpret_cast<unsigned char*>(emalloc(length));
		memcpy(object->writer.buffer, buffer, length);
	}
}


static zend_object* writer_new(zend_class_entry* ce) {
	auto object = alloc_custom_zend_object<byte_buffer_writer_zend_object>(ce, &byte_buffer_writer_zend_object_handlers);

	writer_init_properties(object, nullptr, 0, 0);

	return &object->std;
}

static zend_object* writer_clone(zend_object* object) {
	auto old_object = fetch_from_zend_object<byte_buffer_writer_zend_object>(object);
	auto new_object = alloc_custom_zend_object<byte_buffer_writer_zend_object>(object->ce, &byte_buffer_writer_zend_object_handlers);

	writer_init_properties(new_object, old_object->writer.buffer, old_object->writer.used, old_object->writer.offset);

	zend_objects_clone_members(&new_object->std, &old_object->std);

	return &new_object->std;
}

static void writer_free(zend_object* std) {
	auto object = fetch_from_zend_object<byte_buffer_writer_zend_object>(std);

	efree(object->writer.buffer);
}

static int writer_compare_objects(zval* obj1, zval* obj2) {
	if (Z_TYPE_P(obj1) == IS_OBJECT && Z_TYPE_P(obj2) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE_P(obj1), byte_buffer_writer_ce) && instanceof_function(Z_OBJCE_P(obj2), byte_buffer_writer_ce)) {
			auto object1 = fetch_from_zend_object<byte_buffer_writer_zend_object>(Z_OBJ_P(obj1));
			auto object2 = fetch_from_zend_object<byte_buffer_writer_zend_object>(Z_OBJ_P(obj2));

			if (
				object1->writer.offset == object2->writer.offset &&
				object1->writer.used == object2->writer.used &&
				memcmp(object1->writer.buffer, object2->writer.buffer, object1->writer.used) == 0
			) {
				return 0;
			}
		}
	}

	return 1;
}

#define WRITER_METHOD(name) PHP_METHOD(pmmp_encoding_ByteBufferWriter, name)

WRITER_METHOD(__construct) {
	zend_string* buffer = NULL;
	byte_buffer_writer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(buffer)
	ZEND_PARSE_PARAMETERS_END();

	object = WRITER_THIS();
	if (object->writer.buffer) {
		efree(object->writer.buffer);
	}

	if (buffer == NULL) {
		buffer = zend_empty_string;
	}

	//write offset is placed at the end, as if the given string was written using writeByteArray()
	writer_init_properties(object, reinterpret_cast<unsigned char*>(ZSTR_VAL(buffer)), ZSTR_LEN(buffer), ZSTR_LEN(buffer));
}

WRITER_METHOD(getData) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	RETURN_STRINGL(reinterpret_cast<const char*>(object->writer.buffer), object->writer.used);
}

WRITER_METHOD(writeByteArray) {
	zend_string* value;
	byte_buffer_writer_zend_object* object;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_STR(value)
		ZEND_PARSE_PARAMETERS_END();


	object = WRITER_THIS();

	auto size = ZSTR_LEN(value);

	extendBuffer(object->writer.buffer, object->writer.length, object->writer.offset, size);
	memcpy(&object->writer.buffer[object->writer.offset], ZSTR_VAL(value), size);
	object->writer.offset += size;
	if (object->writer.offset > object->writer.used) {
		object->writer.used = object->writer.offset;
	}
}

WRITER_METHOD(getOffset) {
	zend_parse_parameters_none_throw();
	auto object = WRITER_THIS();
	RETURN_LONG(object->writer.offset);
}

WRITER_METHOD(setOffset) {
	zend_long offset;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	auto object = WRITER_THIS();
	if (offset < 0 || static_cast<size_t>(offset) > object->writer.used) {
		zend_value_error("Offset must not be less than zero or greater than the buffer size");
		return;
	}

	object->writer.offset = static_cast<size_t>(offset);
}

WRITER_METHOD(getUsedLength) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	RETURN_LONG(object->writer.used);
}

WRITER_METHOD(getReservedLength) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	RETURN_LONG(object->writer.length); //don't count null terminator
}

WRITER_METHOD(reserve) {
	zend_long zlength;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
		Z_PARAM_LONG(zlength)
		ZEND_PARSE_PARAMETERS_END();

	if (zlength <= 0) {
		zend_value_error("Length must be greater than zero");
		return;
	}
	auto object = WRITER_THIS();
	extendBuffer(object->writer.buffer, object->writer.length, static_cast<size_t>(zlength), 0);
}

WRITER_METHOD(trim) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	if (object->writer.length > object->writer.used) {
		object->writer.buffer = reinterpret_cast<unsigned char*>(erealloc(object->writer.buffer, object->writer.used));
		object->writer.length = object->writer.used;
	}
}

WRITER_METHOD(clear) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	object->writer.offset = 0;
	object->writer.used = 0;
}

WRITER_METHOD(__serialize) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	array_init(return_value);

	//don't return the writer buffer directly - it may have uninitialized reserved memory
	add_assoc_stringl(return_value, "buffer", reinterpret_cast<char*>(object->writer.buffer), object->writer.used);
	add_assoc_long(return_value, "offset", object->writer.offset);
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

WRITER_METHOD(__unserialize) {
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

	auto object = WRITER_THIS();

	writer_init_properties(object, reinterpret_cast<unsigned char*>(Z_STRVAL_P(buffer)), Z_STRLEN_P(buffer), Z_LVAL_P(offset));
}

WRITER_METHOD(__debugInfo) {
	zend_parse_parameters_none_throw();

	auto object = WRITER_THIS();
	array_init(return_value);

	//don't return the writer buffer directly - it may have uninitialized reserved memory
	add_assoc_stringl(return_value, "buffer", reinterpret_cast<char*>(object->writer.buffer), object->writer.used);
	add_assoc_long(return_value, "offset", object->writer.offset);
}

zend_class_entry* init_class_ByteBufferWriter(void) {
	byte_buffer_writer_ce = register_class_pmmp_encoding_ByteBufferWriter();
	byte_buffer_writer_ce->create_object = writer_new;

	byte_buffer_writer_zend_object_handlers = *zend_get_std_object_handlers();
	byte_buffer_writer_zend_object_handlers.offset = XtOffsetOf(byte_buffer_writer_zend_object, std);
	byte_buffer_writer_zend_object_handlers.clone_obj = writer_clone;
	byte_buffer_writer_zend_object_handlers.free_obj = writer_free;
	byte_buffer_writer_zend_object_handlers.compare = writer_compare_objects;

	return byte_buffer_writer_ce;
}
