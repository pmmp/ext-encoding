--TEST--
Test that new ByteBuffer() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("abc");
var_dump($buffer->toString());

$buffer = new ByteBuffer();
var_dump($buffer->toString());

$buffer = new ByteBuffer("hello world");
$buffer->setReadOffset(6);
var_dump($buffer->readByteArray(5));
?>
--EXPECT--
string(3) "abc"
string(0) ""
string(5) "world"
