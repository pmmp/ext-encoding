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

$functions = [
	'readUnsignedShortLE' => "\xff\xff",
	'readSignedShortLE' => "\xff\xff",
	'readUnsignedShortBE' => "\xff\xff",
	'readSignedShortBE' => "\xff\xff",

	'readUnsignedIntLE' => "\xff\xff\xff\xff",
	'readSignedIntLE' => "\xff\xff\xff\xff",
	'readFloatLE' => "\x11\xff\xff\x11",
	'readUnsignedIntBE' => "\xff\xff\xff\xff",
	'readSignedIntBE' => "\xff\xff\xff\xff",
	'readFloatBE' => "\x11\xff\xff\x11",

	'readSignedLongLE' => "\xff\xff\xff\xff\xff\xff\xff\xff",
	'readSignedLongBE' => "\xff\xff\xff\xff\xff\xff\xff\xff",
	'readDoubleLE' => "\x11\xff\xff\x11\x11\xff\xff\x11",
	'readDoubleBE' => "\x11\xff\xff\x11\x11\xff\xff\x11",
];

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
