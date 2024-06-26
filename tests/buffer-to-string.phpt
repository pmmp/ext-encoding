--TEST--
Test that ByteBuffer::toString() doesn't show unused bytes in reserved space
--DESCRIPTION--
ByteBuffer may allocate more bytes than it needs in order to minimize allocations.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\BE;
use pmmp\encoding\Byte;

$buffer = new ByteBuffer("");
BE::writeSignedInt($buffer, 0); //first buffer alloc, 4 bytes
Byte::writeSigned($buffer, 0); //second buffer alloc, 8 bytes (used 5)

var_dump(bin2hex($buffer->toString()));

$buffer = new ByteBuffer("aaaaaaaaaa");
var_dump($buffer->toString());
?>
--EXPECT--
string(10) "0000000000"
string(10) "aaaaaaaaaa"
