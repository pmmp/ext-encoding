--TEST--
read(Un)SignedVar(Int|Long)() must correctly update the reference $offset parameter if given
--EXTENSIONS--
encoding
--FILE--
<?php

function test(string $function, int $size) : void{
	$varint = str_repeat(str_repeat("\x80", $size - 1) . "\x00", 3);
	$offset = $size;
	$originalOffset = $offset;

	(new ByteBuffer($varint))->$function($offset);
	var_dump($offset === $size + $originalOffset);
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
