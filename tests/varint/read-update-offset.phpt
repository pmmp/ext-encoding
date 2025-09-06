--TEST--
VarInt::read(Un)Signed(Int|Long)() must correctly update the internal offset
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\VarInt;

function test(\Closure $function, int $size) : void{
	$varint = str_repeat(str_repeat("\x80", $size - 1) . "\x00", 3);
	$buffer = new ByteBuffer($varint);
	$originalOffset = $size;
	$buffer->setReadOffset($originalOffset);

	$function($buffer);
	var_dump($buffer->getReadOffset() === $size + $originalOffset);
}

test(VarInt::readUnsignedInt(...), 5);
test(VarInt::readSignedInt(...), 5);
test(VarInt::readUnsignedLong(...), 10);
test(VarInt::readSignedLong(...), 10);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
