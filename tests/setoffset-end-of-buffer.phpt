--TEST--
Test that ByteBuffer::setOffset() allows setting offset at the end of the buffer
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("");
$buffer->setOffset(0);

$buffer->writeByteArray("aaaa");
$buffer->setOffset(4);
$buffer->writeByteArray("bbbb");

var_dump($buffer->toString());
?>
--EXPECT--
string(8) "aaaabbbb"
