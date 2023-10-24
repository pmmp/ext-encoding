--TEST--
read*() for fixed-size type must throw DataDecodeException when not enough bytes are available after the offset given
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\DataDecodeException;

$functions = require __DIR__ . '/fixed-size-types.inc';

$test = str_repeat("\x00", 16);
foreach($functions as $function => $buf){
	try{
		$buffer = new ByteBuffer("\x00");
		$buffer->$function();
	}catch(DataDecodeException $e){
		echo "$function no offset: " . $e->getMessage() . "\n";
	}

	$offset = 15;
	try{
		$buffer = new ByteBuffer($test);
		$buffer->$function($offset);
	}catch(DataDecodeException $e){
		echo "$function with offset: " . $e->getMessage() . "\n";
	}

	echo "\n";
}

?>
--EXPECT--
readUnsignedShortLE no offset: Need at least 2 bytes, but only have 1 bytes
readUnsignedShortLE with offset: Need at least 2 bytes, but only have 1 bytes

readSignedShortLE no offset: Need at least 2 bytes, but only have 1 bytes
readSignedShortLE with offset: Need at least 2 bytes, but only have 1 bytes

readUnsignedShortBE no offset: Need at least 2 bytes, but only have 1 bytes
readUnsignedShortBE with offset: Need at least 2 bytes, but only have 1 bytes

readSignedShortBE no offset: Need at least 2 bytes, but only have 1 bytes
readSignedShortBE with offset: Need at least 2 bytes, but only have 1 bytes

readUnsignedIntLE no offset: Need at least 4 bytes, but only have 1 bytes
readUnsignedIntLE with offset: Need at least 4 bytes, but only have 1 bytes

readSignedIntLE no offset: Need at least 4 bytes, but only have 1 bytes
readSignedIntLE with offset: Need at least 4 bytes, but only have 1 bytes

readFloatLE no offset: Need at least 4 bytes, but only have 1 bytes
readFloatLE with offset: Need at least 4 bytes, but only have 1 bytes

readUnsignedIntBE no offset: Need at least 4 bytes, but only have 1 bytes
readUnsignedIntBE with offset: Need at least 4 bytes, but only have 1 bytes

readSignedIntBE no offset: Need at least 4 bytes, but only have 1 bytes
readSignedIntBE with offset: Need at least 4 bytes, but only have 1 bytes

readFloatBE no offset: Need at least 4 bytes, but only have 1 bytes
readFloatBE with offset: Need at least 4 bytes, but only have 1 bytes

readSignedLongLE no offset: Need at least 8 bytes, but only have 1 bytes
readSignedLongLE with offset: Need at least 8 bytes, but only have 1 bytes

readSignedLongBE no offset: Need at least 8 bytes, but only have 1 bytes
readSignedLongBE with offset: Need at least 8 bytes, but only have 1 bytes

readDoubleLE no offset: Need at least 8 bytes, but only have 1 bytes
readDoubleLE with offset: Need at least 8 bytes, but only have 1 bytes

readDoubleBE no offset: Need at least 8 bytes, but only have 1 bytes
readDoubleBE with offset: Need at least 8 bytes, but only have 1 bytes
