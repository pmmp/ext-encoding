--TEST--
Test that ByteBuffer::getUnreadLength() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("hello world");
var_dump($buffer->getUnreadLength());

$buffer = new ByteBuffer();
var_dump($buffer->getUnreadLength());

$buffer->writeByteArray("hello world");
var_dump($buffer->getUnreadLength()); //zero, because read and write share the same offset (perhaps a bad idea?)

$buffer->rewind();
var_dump($buffer->getUnreadLength());

$buffer->readByteArray(6);
var_dump($buffer->getUnreadLength());

$buffer->readByteArray($buffer->getUnreadLength());
var_dump($buffer->getUnreadLength());
?>
--EXPECT--
int(11)
int(0)
int(0)
int(11)
int(5)
int(0)
