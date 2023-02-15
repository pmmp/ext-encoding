--TEST--
Test that ByteBuffer::toString() doesn't show unused bytes in reserved space
--DESCRIPTION--
ByteBuffer may allocate more bytes than it needs in order to minimize allocations.
--FILE--
<?php

$buffer = new ByteBuffer("");
$buffer->writeIntBE(0); //first buffer alloc, 4 bytes
$buffer->writeByte(0); //second buffer alloc, 8 bytes (used 5)

var_dump(bin2hex($buffer->toString()));
?>
--EXPECT--
string(10) "0000000000"
