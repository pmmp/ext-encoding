--TEST--
read(Un)SignedVar(Int|Long)() must correctly update the internal offset
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\Types;

function test(\Closure $function, int $size) : void{
	$varint = str_repeat(str_repeat("\x80", $size - 1) . "\x00", 3);
	$buffer = new ByteBuffer($varint);
	$originalOffset = $size;
	$buffer->setReadOffset($originalOffset);

	$function($buffer);
	var_dump($buffer->getReadOffset() === $size + $originalOffset);
}

test(Types::readUnsignedVarInt(...), 5);
test(Types::readSignedVarInt(...), 5);
test(Types::readUnsignedVarLong(...), 10);
test(Types::readSignedVarLong(...), 10);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
