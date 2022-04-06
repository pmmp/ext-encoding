--TEST--
read*() must read from the offset given, or the start if none
--FILE--
<?php

function test(\Closure $c, string $buffer) : void{
	$paddingSize = 4;
	$paddedBuffer = str_repeat("\x00", $paddingSize) . $buffer;


	$offset = 0;
	$normalValue = $c($buffer, $offset);

	$offset = $paddingSize;
	$paddedValue = $c($paddedBuffer, $offset);

	$offset = $paddingSize - 1;
	$paddedBadValue = $c($paddedBuffer, $offset);

	var_dump($normalValue === $paddedValue); //true
	var_dump($normalValue === $paddedBadValue); //false
}

$functions = require __DIR__ . '/fixed-size-types.inc';

foreach($functions as $function => $size){
	test(\Closure::fromCallable($function), $size);
}

--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
