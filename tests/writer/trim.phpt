--TEST--
Test that ByteBufferWriter::trim() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;

$buffer = new ByteBufferWriter("");
$buffer->reserve(100);
$buffer->writeByteArray("aaaaa");
var_dump($buffer->getReservedLength());
$buffer->trim();
var_dump($buffer->getReservedLength());

$buffer = new ByteBufferWriter("aaaaaaaaaa");
$buffer->trim();
var_dump($buffer->getReservedLength());
?>
--EXPECT--
int(100)
int(5)
int(10)
