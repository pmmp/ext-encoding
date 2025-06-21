--TEST--
Test that reading varlongs (> 5 bytes) works correctly
--DESCRIPTION--
The result of the bitwise AND operation has a result of int regardless of the input.
When shifting this to the left by more than 31, the result will be 0 if not explicitly
casted to a larger type before shifting. For varlongs this led to all bits above 32
to be discarded. This test verifies the result of the fix.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\VarInt;

$buffer_2 = str_repeat("\x81", 8) . "\x01";
$buf_2 = new ByteBuffer($buffer_2);
var_dump(VarInt::readUnsignedLong($buf_2));

?>
--EXPECT--
int(72624976668147841)
