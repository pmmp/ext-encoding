--TEST--
Test that reserving works correctly
--FILE--
<?php

$buffer = new ByteBuffer("");
var_dump($buffer->getReserved()); //none

$buffer->reserve(40);
var_dump($buffer->getReserved()); //40
var_dump($buffer->toString()); //still empty, we haven't used any space

$buffer->writeSignedByte(ord("a"));
var_dump($buffer->getReserved()); //40
var_dump($buffer->toString());

$buffer->writeByteArray(str_repeat("a", 40)); //cause new allocation, this should double the buffer size to 80
var_dump($buffer->getReserved()); //80
var_dump($buffer->toString());

try{
    $buffer->reserve(-1);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
int(0)
int(40)
string(0) ""
int(40)
string(1) "a"
int(80)
string(41) "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
Length must be greater than zero
