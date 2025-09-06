--TEST--
Test that new ByteBufferReader() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$buffer = new ByteBufferReader("abc");
var_dump($buffer->getData());

$buffer = new ByteBufferReader("");
var_dump($buffer->getData());

$buffer = new ByteBufferReader("hello world");
$buffer->setOffset(6);
var_dump($buffer->readByteArray(5));
?>
--EXPECT--
string(3) "abc"
string(0) ""
string(5) "world"
