--TEST--
Test that ByteBuffer::writeByteArray() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("");
$buffer->writeByteArray("12345");

var_dump($buffer->getWriteOffset());
var_dump($buffer->toString());

$buffer->writeByteArray("67");

var_dump($buffer->getWriteOffset());
var_dump($buffer->toString());

$buffer->setWriteOffset(2);
$buffer->writeByteArray("890");
var_dump($buffer->getWriteOffset());
var_dump($buffer->toString());
?>
--EXPECT--
int(5)
string(5) "12345"
int(7)
string(7) "1234567"
int(5)
string(7) "1289067"
