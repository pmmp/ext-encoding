--TEST--
Test that ByteBufferWriter::writeByteArray() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;

$buffer = new ByteBufferWriter("");
$buffer->writeByteArray("12345");

var_dump($buffer->getOffset());
var_dump($buffer->getData());

$buffer->writeByteArray("67");

var_dump($buffer->getOffset());
var_dump($buffer->getData());

$buffer->setOffset(2);
$buffer->writeByteArray("890");
var_dump($buffer->getOffset());
var_dump($buffer->getData());
?>
--EXPECT--
int(5)
string(5) "12345"
int(7)
string(7) "1234567"
int(5)
string(7) "1289067"
