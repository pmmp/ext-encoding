--TEST--
Test that ByteBuffer::trim() works correctly
--FILE--
<?php

$buffer = new ByteBuffer("");
$buffer->reserve(100);
$buffer->writeByteArray("aaaaa");
var_dump($buffer->getReserved());
$buffer->trim();
var_dump($buffer->getReserved());

$buffer = new ByteBuffer("aaaaaaaaaa");
$buffer->trim();
var_dump($buffer->getReserved());
?>
--EXPECT--
int(100)
int(5)
int(10)
