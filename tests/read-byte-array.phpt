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

$offset = 1;
var_dump($buffer->readByteArray(2, $offset));
var_dump($offset);
$offset = 5;
try{
    $buffer->readByteArray(2, $offset);
}catch(\ValueError $e){
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
$offset must be less than the length (5) of the input string, 5 given
