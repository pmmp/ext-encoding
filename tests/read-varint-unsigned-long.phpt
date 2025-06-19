--TEST--
Test that reading 64-bit VarInt works fine.
--DESCRIPTION--
VarInt::readingUnsignedLong(ByteBuffer) processes up to 10 consecutive bytes until the last processed byte has an MSB (most significant bit).
This method unites the processed byte with the acumulator.
Check that the method works correctly with bytes.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\VarInt;

$buffer_1 = chr(1 | 0x80) // 1
    . chr(1 | 0x80)       // 2
    . chr(1 | 0x80)       // 3
    . chr(1 | 0x80)       // 4
    . chr(1);             // 5
$buf_1 = new ByteBuffer($buffer_1);
var_dump(VarInt::readUnsignedLong($buf_1));

$buffer_2 = chr(1 | 0x80) // 1
    . chr(1 | 0x80)       // 2
    . chr(1 | 0x80)       // 3
    . chr(1 | 0x80)       // 4
    . chr(1 | 0x80)       // 5
    . chr(1 | 0x80)       // 6
    . chr(1 | 0x80)       // 7
    . chr(1 | 0x80)       // 8
    . chr(1);             // 9
$buf_2 = new ByteBuffer($buffer_2);
var_dump(VarInt::readUnsignedLong($buf_2));

?>
--EXPECT--
int(270549121)
int(72624976668147841)