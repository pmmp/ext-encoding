--TEST--
Test that ByteBuffer::setOffset() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("");
$buffer->setOffset(0);

$buffer->writeByteArray("aaaa");
//setting offset at the end of the buffer is allowed and results in buffer extension on the next write
$buffer->setOffset(4);
$buffer->writeByteArray("bbbb");

var_dump($buffer->toString());

$buffer->setOffset(6);
$buffer->writeByteArray("cccc");

var_dump($buffer->toString());

try{
	$buffer->setOffset(-1);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

try{
    $buffer->setOffset(11);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
string(8) "aaaabbbb"
string(10) "aaaabbcccc"
Offset must not be less than zero or greater than the buffer size
Offset must not be less than zero or greater than the buffer size
