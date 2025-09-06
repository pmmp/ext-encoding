--TEST--
Test that ByteBufferWriter::getData() doesn't show unused bytes in reserved space
--DESCRIPTION--
ByteBufferWriter may allocate more bytes than it needs in order to minimize allocations.
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\BE;
use pmmp\encoding\Byte;

$buffer = new ByteBufferWriter("");
BE::writeSignedInt($buffer, 0); //first buffer alloc, 4 bytes
Byte::writeSigned($buffer, 0); //second buffer alloc, 8 bytes (used 5)

var_dump(bin2hex($buffer->getData()));

$buffer = new ByteBufferWriter("aaaaaaaaaa");
var_dump($buffer->getData());
?>
--EXPECT--
string(10) "0000000000"
string(10) "aaaaaaaaaa"
