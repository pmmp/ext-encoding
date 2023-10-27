--TEST--
Test that ByteBuffer::readByteArray() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\DataDecodeException;

$buffer = new ByteBuffer("");

//read with no bytes available
try{
    $buffer->readByteArray(1);
}catch(DataDecodeException $e){
    echo $e->getMessage() . PHP_EOL;
}

$buffer = new ByteBuffer("abcde");
var_dump($buffer->readByteArray(3));
var_dump($buffer->readByteArray(1));
try{
    $buffer->readByteArray(2);
}catch(DataDecodeException $e){
    echo $e->getMessage() . PHP_EOL;
}

$buffer = new ByteBuffer("abcde");
try{
    $buffer->readByteArray(-1);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

//ensure offset is updated properly
$buffer->setOffset(1);
var_dump($buffer->readByteArray(2));
var_dump($buffer->getOffset());

//read with bytes, but all before the buffer start
$buffer->setOffset(5);
try{
    $buffer->readByteArray(2);
}catch(DataDecodeException $e){
    echo $e->getMessage() . PHP_EOL;
}
--EXPECT--
Need at least 1 bytes, but only have 0 bytes
string(3) "abc"
string(1) "d"
Need at least 2 bytes, but only have 1 bytes
Length cannot be negative
string(2) "bc"
int(3)
Need at least 2 bytes, but only have 0 bytes
