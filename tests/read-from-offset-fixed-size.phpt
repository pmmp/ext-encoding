--TEST--
read*() must read from the offset given, or the start if none
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

function test(string $function, string $buffer) : void{
	$paddingSize = 4;
	$paddedBuffer = str_repeat("\x00", $paddingSize) . $buffer;


	$offset = 0;
	$normalValue = (new ByteBuffer($buffer))->$function($offset);

	$offset = 0;
	$noOffsetValue = (new ByteBuffer($buffer))->$function();

	$offset = $paddingSize;
	$paddedValue = (new ByteBuffer($paddedBuffer))->$function($offset);

	$offset = $paddingSize - 1;
	$paddedBadValue = (new ByteBuffer($paddedBuffer))->$function($offset);

	var_dump($normalValue === $noOffsetValue); //true
	var_dump($normalValue === $paddedValue); //true
	var_dump($normalValue === $paddedBadValue); //false
}

$functions = require __DIR__ . '/fixed-size-types.inc';

foreach($functions as $function => $size){
	test($function, $size);
}

--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
