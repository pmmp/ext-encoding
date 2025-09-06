--TEST--
read(Un)SignedVar(Int|Long)() must terminate when too many bytes are given
--DESCRIPTION--
VarInt reading works by checking if the MSB (most significant bit) is set on the current byte.
If it is, it reads another byte.

However, this means that it's possible for a string of bytes longer than the max size of a varint/varlong to appear,
potentially locking up the read for a long time if the max number of bytes isn't capped.

This test verifies that the varint reader functions bail out if there are too many consecutive bytes with MSB set.
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\DataDecodeException;
use pmmp\encoding\VarInt;

$shortBuf = str_repeat("\x80", 6);
$longBuf = str_repeat("\x80", 11);

try{
	VarInt::readUnsignedInt(new ByteBufferReader($shortBuf));
}catch(DataDecodeException $e){
	echo "uv32: " . $e->getMessage() . PHP_EOL;
}

try{
	VarInt::readSignedInt(new ByteBufferReader($shortBuf));
}catch(DataDecodeException $e){
	echo "sv32: " . $e->getMessage() . PHP_EOL;
}

try{
	VarInt::readUnsignedLong(new ByteBufferReader($longBuf));
}catch(DataDecodeException $e){
	echo "uv64: " . $e->getMessage() . PHP_EOL;
}

try{
	VarInt::readSignedLong(new ByteBufferReader($longBuf));
}catch(DataDecodeException $e){
	echo "sv64: " . $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
uv32: VarInt did not terminate after 5 bytes!
sv32: VarInt did not terminate after 5 bytes!
uv64: VarInt did not terminate after 10 bytes!
sv64: VarInt did not terminate after 10 bytes!
