--TEST--
Test ByteBuffer::getUnreadLength()
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$buffer = new ByteBufferReader("hello world");
var_dump($buffer->getUnreadLength()); //11

$buffer->readByteArray(1);
var_dump($buffer->getUnreadLength()); //10

$buffer->setOffset(5);
var_dump($buffer->getUnreadLength()); //6

$buffer->readByteArray(6);
var_dump($buffer->getUnreadLength()); //0

$buffer->setOffset(11); //length of buffer
var_dump($buffer->getUnreadLength()); //0

$buffer->setOffset(0);
var_dump($buffer->getUnreadLength()); //11
?>
--EXPECT--
int(11)
int(10)
int(6)
int(0)
int(0)
int(11)
