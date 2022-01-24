--TEST--
read*() for fixed-size type must correctly handle being given invalid offset values
--EXTENSIONS--
encoding
--FILE--
<?php

function test(\Closure $c) : void{
	try{
		$offset = 4;
		$c("\x00", $offset);
	}catch(ValueError $e){
		echo "offset larger than buffer size: " . $e->getMessage() . "\n";
	}

	try{
		$offset = -4;
		$c("\x00", $offset);
	}catch(ValueError $e){
		echo "offset negative: " . $e->getMessage() . "\n";
	}

	try{
		$offset = "1";
		$c("\x00", $offset);
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

$test = str_repeat("\x00", 16);
foreach($functions as $function){
	test(\Closure::fromCallable($function));
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
