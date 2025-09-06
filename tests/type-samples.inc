<?php

use pmmp\encoding\BE;
use pmmp\encoding\LE;
use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\ByteBufferWriter;

enum EndianInts{
	case UNSIGNED_SHORT;
	case SIGNED_SHORT;
	case UNSIGNED_TRIAD;
	case SIGNED_TRIAD;
	case UNSIGNED_INT;
	case SIGNED_INT;
	case UNSIGNED_LONG;
	case SIGNED_LONG;

	public function getMin() : int{
		return match ($this) {
			self::UNSIGNED_SHORT,
			self::UNSIGNED_TRIAD,
			self::UNSIGNED_INT => 0,
			self::SIGNED_SHORT => -32768,
			self::SIGNED_TRIAD => -8388608,
			self::SIGNED_INT => -2147483648,

			self::UNSIGNED_LONG, //PHP doesn't have an unsigned long type
			self::SIGNED_LONG => -9223372036854775807 - 1, //PHP parser weirdness
		};
	}

	public function getMax() : int{
		return match ($this) {
			self::UNSIGNED_SHORT => 65535,
			self::UNSIGNED_TRIAD => 16777215,
			self::UNSIGNED_INT => 4294967295,
			self::SIGNED_SHORT => 32767,
			self::SIGNED_TRIAD => 8388607,
			self::SIGNED_INT => 2147483647,

			self::UNSIGNED_LONG, //PHP doesn't have an unsigned long type
			self::SIGNED_LONG => 9223372036854775807,
		};
	}

	/**
	 * @return int[]
	 */
	public function getSamples() : array{
		$min = $this->getMin();
		$signed = $min < 0;
		$max = $this->getMax();
		return $signed ? [$min, $min + 1, -1, 0, 1, $max - 1, $max] : [0, 1, $max - 1, $max];
	}

	/**
	 * @phpstan-return array{\Closure(ByteBufferReader):int, Closure(ByteBufferWriter,int):void, Closure(ByteBufferReader,int):array<int>, Closure(ByteBufferWriter,array<int>):void}
	 */
	function getMethods(bool $bigEndian) : array{
		$class = $bigEndian ? BE::class : LE::class;
		return match($this){
			self::UNSIGNED_SHORT => [$class::readUnsignedShort(...), $class::writeUnsignedShort(...), $class::readUnsignedShortArray(...), $class::writeUnsignedShortArray(...)],
			self::SIGNED_SHORT => [$class::readSignedShort(...), $class::writeSignedShort(...), $class::readSignedShortArray(...), $class::writeSignedShortArray(...)],
			self::UNSIGNED_TRIAD => [$class::readUnsignedTriad(...), $class::writeUnsignedTriad(...), $class::readUnsignedTriadArray(...), $class::writeUnsignedTriadArray(...)],
			self::SIGNED_TRIAD => [$class::readSignedTriad(...), $class::writeSignedTriad(...), $class::readSignedTriadArray(...), $class::writeSignedTriadArray(...)],
			self::UNSIGNED_INT => [$class::readUnsignedInt(...), $class::writeUnsignedInt(...), $class::readUnsignedIntArray(...), $class::writeUnsignedIntArray(...)],
			self::SIGNED_INT => [$class::readSignedInt(...), $class::writeSignedInt(...), $class::readSignedIntArray(...), $class::writeSignedIntArray(...)],
			self::UNSIGNED_LONG => [$class::readUnsignedLong(...), $class::writeUnsignedLong(...), $class::readUnsignedLongArray(...), $class::writeUnsignedLongArray(...)],
			self::SIGNED_LONG => [$class::readSignedLong(...), $class::writeSignedLong(...), $class::readSignedLongArray(...), $class::writeSignedLongArray(...)],
		};
	}
}

enum FloatSamples{
	case FLOAT;
	case DOUBLE;

	public function getMethods(bool $bigEndian) : array{
		$class = $bigEndian ? BE::class : LE::class;
		return match($this){
			self::FLOAT => [$class::readFloat(...), $class::writeFloat(...), $class::readFloatArray(...), $class::writeFloatArray(...)],
			self::DOUBLE => [$class::readDouble(...), $class::writeDouble(...), $class::readDoubleArray(...), $class::writeDoubleArray(...)],
		};
	}
}
