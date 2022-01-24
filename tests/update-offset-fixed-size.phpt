--TEST--
read*() for fixed-size type must correctly update the reference $offset parameter if given
--EXTENSIONS--
encoding
--FILE--
<?php

function test(\Closure $c, int $size) : void{
	$offset = $size;
	$originalOffset = $offset;
	$buffer = str_repeat("\x00", $size * 3);

	$c($buffer, $offset);
	var_dump($offset === $originalOffset + $size);
}

$functions = [
	'readUnsignedShortLE' => 2,
	'readSignedShortLE' => 2,
	'readUnsignedShortBE' => 2,
	'readSignedShortBE' => 2,

	'readUnsignedIntLE' => 4,
	'readSignedIntLE' => 4,
	'readFloatLE' => 4,
	'readUnsignedIntBE' => 4,
	'readSignedIntBE' => 4,
	'readFloatBE' => 4,

	'readSignedLongLE' => 8,
	'readSignedLongBE' => 8,
	'readDoubleLE' => 8,
	'readDoubleBE' => 8,
];

foreach($functions as $function => $size){
	test(\Closure::fromCallable($function), $size);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
