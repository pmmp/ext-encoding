--TEST--
read*() for fixed-size type must throw DataDecodeException when not enough bytes are available after the offset given
--EXTENSIONS--
encoding
--FILE--
<?php

$functions = [
	'readUnsignedShortLE',
	'readSignedShortLE',
	'readUnsignedShortBE',
	'readSignedShortBE',

	'readUnsignedIntLE',
	'readSignedIntLE',
	'readFloatLE',
	'readUnsignedIntBE',
	'readSignedIntBE',
	'readFloatBE',

	'readSignedLongLE',
	'readSignedLongBE',
	'readDoubleLE',
	'readDoubleBE',
];

$test = str_repeat("\x00", 16);
foreach($functions as $function){
	$offset = 15;
	try{
		$function($test, $offset);
	}catch(DataDecodeException $e){
		echo "$function: " . $e->getMessage() . "\n";
	}
}

?>
--EXPECT--
readUnsignedShortLE: Need at least 2 bytes, but only have 1 bytes
readSignedShortLE: Need at least 2 bytes, but only have 1 bytes
readUnsignedShortBE: Need at least 2 bytes, but only have 1 bytes
readSignedShortBE: Need at least 2 bytes, but only have 1 bytes
readUnsignedIntLE: Need at least 4 bytes, but only have 1 bytes
readSignedIntLE: Need at least 4 bytes, but only have 1 bytes
readFloatLE: Need at least 4 bytes, but only have 1 bytes
readUnsignedIntBE: Need at least 4 bytes, but only have 1 bytes
readSignedIntBE: Need at least 4 bytes, but only have 1 bytes
readFloatBE: Need at least 4 bytes, but only have 1 bytes
readSignedLongLE: Need at least 8 bytes, but only have 1 bytes
readSignedLongBE: Need at least 8 bytes, but only have 1 bytes
readDoubleLE: Need at least 8 bytes, but only have 1 bytes
readDoubleBE: Need at least 8 bytes, but only have 1 bytes