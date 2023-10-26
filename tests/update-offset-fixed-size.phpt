--TEST--
read*() for fixed-size type must correctly update the reference $offset parameter if given
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

function test(string $function, int $size) : void{
	$offset = $size;
	$originalOffset = $offset;
	$buffer = str_repeat("\x00", $size * 3);

	(new ByteBuffer($buffer))->$function($offset);
	var_dump($offset === $originalOffset + $size);
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
