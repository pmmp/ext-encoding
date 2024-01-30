--TEST--
Test that ByteBuffer::trim() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("");
$buffer->reserve(100);
$buffer->writeByteArray("aaaaa");
var_dump($buffer->getReservedLength());
$buffer->trim();
var_dump($buffer->getReservedLength());

$buffer = new ByteBuffer("aaaaaaaaaa");
$buffer->trim();
var_dump($buffer->getReservedLength());
?>
--EXPECT--
int(100)
int(5)
int(10)
