--TEST--
read(Un)SignedVar(Int|Long)() must correctly update the reference $offset parameter if given
--EXTENSIONS--
encoding
--FILE--
<?php

function test(\Closure $c, int $size) : void{
	$varint = str_repeat(str_repeat("\x80", $size - 1) . "\x00", 3);
	$offset = $size;
	$originalOffset = $offset;

	$c($varint, $offset);
	var_dump($offset === $size + $originalOffset);
}

test(\Closure::fromCallable('readUnsignedVarInt'), 5);
test(\Closure::fromCallable('readSignedVarInt'), 5);
test(\Closure::fromCallable('readUnsignedVarLong'), 10);
test(\Closure::fromCallable('readSignedVarLong'), 10);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
