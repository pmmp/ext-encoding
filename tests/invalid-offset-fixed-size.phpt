--TEST--
read*() for fixed-size type must correctly handle being given invalid offset values
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

function test(string $function) : void{
	try{
		$offset = 4;
		(new ByteBuffer("\x00"))->$function($offset);
	}catch(ValueError $e){
		echo "offset larger than buffer size: " . $e->getMessage() . "\n";
	}

	try{
		$offset = -4;
		(new ByteBuffer("\x00"))->$function($offset);
	}catch(ValueError $e){
		echo "offset negative: " . $e->getMessage() . "\n";
	}

	try{
		$offset = "1";
		(new ByteBuffer("\x00"))->$function($offset);
	}catch(TypeError $e){
		echo "wrong type offset: " . $e->getMessage() . "\n";
	}
	
	echo "\n";
}

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

foreach($functions as $function){
	test($function);
}

?>
--EXPECT--
offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given
