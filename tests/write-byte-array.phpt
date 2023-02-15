--TEST--
Test that ByteBuffer::writeByteArray() works as expected
--FILE--
<?php

$buffer = new ByteBuffer("");
$buffer->writeByteArray("12345");

var_dump($buffer->getOffset());
var_dump($buffer->toString());

$buffer->writeByteArray("67");

var_dump($buffer->getOffset());
var_dump($buffer->toString());

$offset = 2;
$buffer->writeByteArray("890", $offset);
var_dump($offset); //5
var_dump($buffer->getOffset()); //unchanged
var_dump($buffer->toString());
?>
--EXPECT--
int(5)
string(5) "12345"
int(7)
string(7) "1234567"
int(5)
int(7)
string(7) "1289067"
