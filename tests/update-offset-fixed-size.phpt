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

$functions = require __DIR__ . '/fixed-size-types.inc';

foreach($functions as $function => $buf){
	test(\Closure::fromCallable($function), strlen($buf));
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
