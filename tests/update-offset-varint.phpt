--TEST--
read(Un)SignedVar(Int|Long)() must correctly update the internal offset
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

function test(string $function, int $size) : void{
	$varint = str_repeat(str_repeat("\x80", $size - 1) . "\x00", 3);
	$buffer = new ByteBuffer($varint);
	$originalOffset = $size;
	$buffer->setReadOffset($originalOffset);

	$buffer->$function();
	var_dump($buffer->getReadOffset() === $size + $originalOffset);
}

test('readUnsignedVarInt', 5);
test('readSignedVarInt', 5);
test('readUnsignedVarLong', 10);
test('readSignedVarLong', 10);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
