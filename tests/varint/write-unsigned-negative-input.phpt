--TEST--
writeUnsignedVar(Int|Long)() must terminate when given a negative number
--DESCRIPTION--
Due to the use of arithmetic right-shift for extracting bits from a number for varint encoding (PHP has no unsigned
types, nor a logical right-shift operator), BinaryUtils in the early days of varints ran into a bug where encoding
a negative varint as unsigned would never terminate, because -1 >> anything = -1.

This should not be an issue in ext-encoding because of the exclusive use of unsigned integer types for the actual
encoding (meaning that the compiler always generates logical right-shift instructions), but this needs to be tested
anyway.

In such cases, we expect that the type will be truncated to the appropriate size, and the remainder of the bits
treated as unsigned. This means that -1 written as a varint32 will be interpreted as 4,294,967,295 and so on.

In the varint64 case we would not truncate, but still interpret the number as its unsigned counterpart during writing.
However, during decoding this would lead to the MSB being interpreted as a sign bit, which also happens for actual
unsigned numbers. Due to the lack of unsigned types in PHP, there isn't much that can be done about this.
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\VarInt;

$buffer = new ByteBufferWriter("");
VarInt::writeUnsignedInt($buffer, -1);
$reader = new ByteBufferReader($buffer->getData());
var_dump(VarInt::readUnsignedInt($reader));

$buffer->setOffset(0);
VarInt::writeUnsignedLong($buffer, -1);
$reader = new ByteBufferReader($buffer->getData());
var_dump(VarInt::readUnsignedLong($reader));

?>
--EXPECT--
int(4294967295)
int(-1)

