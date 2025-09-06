#ifndef SERIALIZERS_H
#define SERIALIZERS_H

extern "C" {
#include "Zend/zend_string.h"
#include "Zend/zend_exceptions.h"
}
#include "classes/DataDecodeException.h"
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

enum class ByteOrder {
	BigEndian,
	LittleEndian,
#ifdef WORDS_BIGENDIAN
	Native = BigEndian
#else
	Native = LittleEndian
#endif
};

template<typename TValue>
union Flipper {
	char bytes[sizeof(TValue)];
	TValue value;
};

template<typename TValue>
static inline bool readByte(unsigned char* buffer, size_t used, size_t& offset, TValue& result) {
	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	result = *(reinterpret_cast<TValue*>(&buffer[offset]));

	offset += SIZE;
	return true;
}
template<typename TValue, ByteOrder byteOrder>
static inline bool readFixedSizeType(unsigned char* bytes, size_t used, size_t& offset, TValue& result) {

	const auto SIZE = sizeof(TValue);
	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	Flipper<TValue> flipper;

	memcpy(flipper.bytes, &bytes[offset], sizeof(flipper.bytes));
	if (byteOrder != ByteOrder::Native) {
		std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
	}

	result = flipper.value;

	offset += SIZE;
	return true;
}

template<typename TValue, ByteOrder byteOrder>
static inline bool readFixedSizeTypeArray(unsigned char* bytes, size_t used, size_t& offset, size_t count, std::vector<TValue>& resultArray) {
	size_t sizeBytes = count * sizeof(TValue);

	if (used - offset < sizeBytes) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", sizeBytes, used - offset);
		return false;
	}

	TValue* rawValues = reinterpret_cast<TValue*>(&bytes[offset]);

	resultArray.assign(rawValues, rawValues + count);
	if (byteOrder != ByteOrder::Native) {
		Flipper<TValue> flipper;

		for (size_t i = 0; i < count; i++) {
			flipper.value = resultArray[i];
			std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
			resultArray[i] = flipper.value;
		}
	}

	offset += sizeBytes;
	return true;
}

template<typename TValue>
using readComplexTypeFunc_t = bool (*) (unsigned char* bytes, size_t used, size_t& offset, TValue& result);

template<typename TValue, readComplexTypeFunc_t<TValue> readComplexTypeFunc>
static inline bool readComplexTypeArray(unsigned char* bytes, size_t used, size_t& offset, size_t count, std::vector<TValue>& resultArray) {
	for (size_t i = 0; i < count; i++) {
		TValue value;

		if (!readComplexTypeFunc(bytes, used, offset, value)) {
			return false;
		}

		resultArray.push_back(value);
	}

	return true;
}

template<typename TValue, ByteOrder byteOrder>
static inline bool readInt24(unsigned char* bytes, size_t used, size_t& offset, TValue& result) {
	const size_t SIZE = 3;

	if (used - offset < SIZE) {
		zend_throw_exception_ex(data_decode_exception_ce, 0, "Need at least %zu bytes, but only have %zu bytes", SIZE, used - offset);
		return false;
	}

	result = 0;
	if (byteOrder == ByteOrder::LittleEndian) {
		result |= bytes[offset];
		result |= bytes[offset + 1] << 8;
		result |= bytes[offset + 2] << 16;
	}
	else {
		result |= bytes[offset + 2];
		result |= bytes[offset + 1] << 8;
		result |= bytes[offset] << 16;
	}

	const size_t SIGNED_SHIFT = std::is_signed<TValue>::value ? (sizeof(TValue) - SIZE) * CHAR_BIT : 0;
	if (SIGNED_SHIFT > 0) {
		result = (result << SIGNED_SHIFT) >> SIGNED_SHIFT;
	}

	offset += SIZE;
	return true;
}

struct VarIntConstants {
	static const unsigned char BITS_PER_BYTE = 7u;

	template<size_t TYPE_BITS>
	static const unsigned char MAX_BYTES = TYPE_BITS / BITS_PER_BYTE + ((TYPE_BITS % BITS_PER_BYTE) > 0);

	static const unsigned char VALUE_MASK = static_cast<unsigned char>(~(1u << BITS_PER_BYTE));
	static const unsigned char MSB_MASK = static_cast<unsigned char>(1u << BITS_PER_BYTE);
};

template<typename TValue>
static inline bool readUnsignedVarInt(unsigned char* bytes, size_t used, size_t& offset, TValue& result) {
	const auto TYPE_BITS = sizeof(TValue) * CHAR_BIT;
	result = 0;
	for (unsigned int shift = 0; shift < TYPE_BITS; shift += VarIntConstants::BITS_PER_BYTE) {
		if (offset >= used) {
			zend_throw_exception(data_decode_exception_ce, "No bytes left in buffer", 0);
			return false;
		}

		auto byte = bytes[offset++];
		result |= static_cast<TValue>(byte & VarIntConstants::VALUE_MASK) << shift;
		if ((byte & VarIntConstants::MSB_MASK) == 0) {
			return true;
		}
	}

	zend_throw_exception_ex(data_decode_exception_ce, 0, "VarInt did not terminate after %u bytes!", VarIntConstants::MAX_BYTES<TYPE_BITS>);
	return false;
}

template<typename TUnsignedValue, typename TSignedValue>
static inline bool readSignedVarInt(unsigned char* bytes, size_t used, size_t& offset, TSignedValue& result) {
	TUnsignedValue unsignedResult;
	if (!readUnsignedVarInt<TUnsignedValue>(bytes, used, offset, unsignedResult)) {
		return false;
	}

	TUnsignedValue mask = 0;
	if (unsignedResult & 1) {
		//we don't know the type of TUnsignedValue here so we can't just use ~0
		mask = ~mask;
	}

	result = static_cast<TSignedValue>((unsignedResult >> 1) ^ mask);
	return true;
}


static inline void extendBuffer(zend_string*& buffer, size_t offset, size_t usedBytes) {
	size_t requiredSize = offset + usedBytes;
	if (ZSTR_LEN(buffer) < requiredSize) {
		size_t doubleSize = ZSTR_LEN(buffer) * 2;
		buffer = zend_string_realloc(buffer, doubleSize > requiredSize ? doubleSize : requiredSize, 0);
		ZSTR_VAL(buffer)[ZSTR_LEN(buffer)] = '\0'; //make sure null terminator is always set, to stop sprintf reading out-of-bounds
	}
	else {
		buffer = zend_string_separate(buffer, 0);
	}
}

template<typename TValue>
static void writeByte(zend_string*& buffer, size_t& offset, TValue value) {
	extendBuffer(buffer, offset, sizeof(TValue));

	ZSTR_VAL(buffer)[offset] = *reinterpret_cast<char*>(&value);

	offset += sizeof(TValue);
}

template<typename TValue, ByteOrder byteOrder>
static void writeFixedSizeType(zend_string*& buffer, size_t& offset, TValue value) {
	extendBuffer(buffer, offset, sizeof(TValue));

	Flipper<TValue> flipper;
	flipper.value = value;

	if (byteOrder != ByteOrder::Native) {
		std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
	}

	memcpy(&ZSTR_VAL(buffer)[offset], flipper.bytes, sizeof(flipper.bytes));

	offset += sizeof(TValue);
}

template<typename TValue, ByteOrder byteOrder>
static void writeFixedSizeTypeArray(zend_string*& buffer, size_t& offset, std::vector<TValue>& valueArray) {
	size_t arraySizeBytes = valueArray.size() * sizeof(TValue);
	extendBuffer(buffer, offset, arraySizeBytes);

	if (byteOrder != ByteOrder::Native) {
		Flipper<TValue> flipper;

		for (size_t i = 0; i < valueArray.size(); i++) {
			flipper.value = valueArray[i];
			std::reverse(std::begin(flipper.bytes), std::end(flipper.bytes));
			valueArray[i] = flipper.value;
		}
	}

	memcpy(&ZSTR_VAL(buffer)[offset], reinterpret_cast<char*>(valueArray.data()), arraySizeBytes);
	offset += arraySizeBytes;
}

template<typename TValue>
using writeComplexTypeFunc_t = void (*) (zend_string*& buffer, size_t& offset, TValue value);

template<typename TValue, writeComplexTypeFunc_t<TValue> writeNaiveTypeFunc>
static void writeComplexTypeArray(zend_string*& buffer, size_t& offset, std::vector<TValue>& valueArray) {
	for (size_t i = 0; i < valueArray.size(); i++) {
		writeNaiveTypeFunc(buffer, offset, valueArray[i]);
	}
}

template<typename TValue, ByteOrder byteOrder>
static void writeInt24(zend_string*& buffer, size_t& offset, TValue value) {
	const size_t SIZE = 3;
	extendBuffer(buffer, offset, SIZE);

	if (byteOrder == ByteOrder::LittleEndian) {
		ZSTR_VAL(buffer)[offset] = value & 0xff;
		ZSTR_VAL(buffer)[offset + 1] = (value >> 8) & 0xff;
		ZSTR_VAL(buffer)[offset + 2] = (value >> 16) & 0xff;
	}
	else {
		ZSTR_VAL(buffer)[offset] = (value >> 16) & 0xff;
		ZSTR_VAL(buffer)[offset + 1] = (value >> 8) & 0xff;
		ZSTR_VAL(buffer)[offset + 2] = value & 0xff;
	}

	offset += SIZE;
}

template<typename TValue>
static inline void writeUnsignedVarInt(zend_string*& buffer, size_t& offset, TValue value) {
	const auto TYPE_BITS = sizeof(TValue) * CHAR_BIT;
	char result[VarIntConstants::MAX_BYTES<TYPE_BITS>];

	TValue remaining = value;

	for (auto i = 0; i < VarIntConstants::MAX_BYTES<TYPE_BITS>; i++) {
		unsigned char nextByte = remaining & VarIntConstants::VALUE_MASK;

		TValue nextRemaining = remaining >> VarIntConstants::BITS_PER_BYTE;

		if (nextRemaining == 0) {
			result[i] = nextByte;

			auto usedBytes = i + 1;
			extendBuffer(buffer, offset, usedBytes);
			memcpy(&ZSTR_VAL(buffer)[offset], &result[0], usedBytes);
			offset += usedBytes;

			return;
		}

		result[i] = nextByte | VarIntConstants::MSB_MASK;
		remaining = nextRemaining;
	}

	zend_value_error("Value too large to be encoded as a VarInt");
}

template<typename TUnsignedType, typename TSignedType>
static inline void writeSignedVarInt(zend_string*& buffer, size_t& offset, TSignedType value) {
	TUnsignedType mask = 0;
	if (value < 0) {
		//we don't know the type of TUnsignedType here, can't use ~0 directly (the compiler will optimise this anyway)
		mask = ~mask;
	}

	writeUnsignedVarInt<TUnsignedType>(buffer, offset, (static_cast<TUnsignedType>(value) << 1) ^ mask);
}

#endif
