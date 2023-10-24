--TEST--
Test that ByteBuffer::read*() can't read reserved unused memory
--DESCRIPTION--
Since we may have allocated more memory than we're currently using, we need to make sure that said extra memory isn't readable, otherwise we will return
uninitialized values to PHP.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\DataDecodeException;

$buffer = new ByteBuffer("");
$buffer->reserve(100);

try{
    var_dump($buffer->readByteArray(10));
}catch(DataDecodeException $e){
    echo $e->getMessage() . PHP_EOL;
}

try{
    var_dump($buffer->readUnsignedByte());
}catch(DataDecodeException $e){
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
Need at least 10 bytes, but only have 0 bytes
Need at least 1 bytes, but only have 0 bytes
