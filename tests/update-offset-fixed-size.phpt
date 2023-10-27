--TEST--
read*() for fixed-size type must correctly update the reference $offset parameter if given
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

function test(string $function, int $size) : void{
	$originalOffset = $size;

	$bytes = str_repeat("\x00", $size * 3);
	$buffer = new ByteBuffer($bytes);
	$buffer->setOffset($originalOffset);

	$buffer->$function();
	var_dump($buffer->getOffset() === $originalOffset + $size);
}

$functions = require __DIR__ . '/fixed-size-types.inc';

foreach($functions as $function => $buf){
	test($function, strlen($buf));
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
bool(true)
bool(true)
bool(true)
bool(true)
