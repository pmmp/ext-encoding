--TEST--
Test that ByteBuffer::setWriteOffset() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("");
$buffer->setWriteOffset(0);

$buffer->writeByteArray("aaaa");
//setting offset at the end of the buffer is allowed and results in buffer extension on the next write
$buffer->setWriteOffset(4);
$buffer->writeByteArray("bbbb");

var_dump($buffer->toString());

$buffer->setWriteOffset(6);
$buffer->writeByteArray("cccc");

var_dump($buffer->toString());

try{
	$buffer->setWriteOffset(-1);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

try{
    $buffer->setWriteOffset(11);
}catch(\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
string(8) "aaaabbbb"
string(10) "aaaabbcccc"
Offset must not be less than zero or greater than the buffer size
Offset must not be less than zero or greater than the buffer size
