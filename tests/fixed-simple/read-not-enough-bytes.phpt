--TEST--
read*() for fixed-size type must throw DataDecodeException when not enough bytes are available
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\DataDecodeException;

$functions = require __DIR__ . '/read-samples.inc';

$test = str_repeat("\x00", 16);
foreach($functions as [$function, $buf]){
	try{
		$buffer = new ByteBufferReader("\x00");
        $function($buffer);
	}catch(DataDecodeException $e){
		$reflect = new \ReflectionFunction($function);

		echo $reflect->getClosureScopeClass()->getShortName() . "::" . $reflect->getName() . " no offset: " . $e->getMessage() . "\n";
	}

	try{
		$buffer = new ByteBufferReader($test);
		$buffer->setOffset(15);
        $function($buffer);
	}catch(DataDecodeException $e){
		$reflect = new \ReflectionFunction($function);

		echo $reflect->getClosureScopeClass()->getShortName() . "::" . $reflect->getName() . " with offset: " . $e->getMessage() . "\n";
	}

	echo "\n";
}

?>
--EXPECT--
LE::readUnsignedShort no offset: Need at least 2 bytes, but only have 1 bytes
LE::readUnsignedShort with offset: Need at least 2 bytes, but only have 1 bytes

LE::readSignedShort no offset: Need at least 2 bytes, but only have 1 bytes
LE::readSignedShort with offset: Need at least 2 bytes, but only have 1 bytes

BE::readUnsignedShort no offset: Need at least 2 bytes, but only have 1 bytes
BE::readUnsignedShort with offset: Need at least 2 bytes, but only have 1 bytes

BE::readSignedShort no offset: Need at least 2 bytes, but only have 1 bytes
BE::readSignedShort with offset: Need at least 2 bytes, but only have 1 bytes

LE::readUnsignedInt no offset: Need at least 4 bytes, but only have 1 bytes
LE::readUnsignedInt with offset: Need at least 4 bytes, but only have 1 bytes

LE::readSignedInt no offset: Need at least 4 bytes, but only have 1 bytes
LE::readSignedInt with offset: Need at least 4 bytes, but only have 1 bytes

LE::readFloat no offset: Need at least 4 bytes, but only have 1 bytes
LE::readFloat with offset: Need at least 4 bytes, but only have 1 bytes

BE::readUnsignedInt no offset: Need at least 4 bytes, but only have 1 bytes
BE::readUnsignedInt with offset: Need at least 4 bytes, but only have 1 bytes

BE::readSignedInt no offset: Need at least 4 bytes, but only have 1 bytes
BE::readSignedInt with offset: Need at least 4 bytes, but only have 1 bytes

BE::readFloat no offset: Need at least 4 bytes, but only have 1 bytes
BE::readFloat with offset: Need at least 4 bytes, but only have 1 bytes

LE::readSignedLong no offset: Need at least 8 bytes, but only have 1 bytes
LE::readSignedLong with offset: Need at least 8 bytes, but only have 1 bytes

BE::readSignedLong no offset: Need at least 8 bytes, but only have 1 bytes
BE::readSignedLong with offset: Need at least 8 bytes, but only have 1 bytes

LE::readDouble no offset: Need at least 8 bytes, but only have 1 bytes
LE::readDouble with offset: Need at least 8 bytes, but only have 1 bytes

BE::readDouble no offset: Need at least 8 bytes, but only have 1 bytes
BE::readDouble with offset: Need at least 8 bytes, but only have 1 bytes

BE::readUnsignedTriad no offset: Need at least 3 bytes, but only have 1 bytes
BE::readUnsignedTriad with offset: Need at least 3 bytes, but only have 1 bytes

LE::readUnsignedTriad no offset: Need at least 3 bytes, but only have 1 bytes
LE::readUnsignedTriad with offset: Need at least 3 bytes, but only have 1 bytes

BE::readSignedTriad no offset: Need at least 3 bytes, but only have 1 bytes
BE::readSignedTriad with offset: Need at least 3 bytes, but only have 1 bytes

LE::readSignedTriad no offset: Need at least 3 bytes, but only have 1 bytes
LE::readSignedTriad with offset: Need at least 3 bytes, but only have 1 bytes
