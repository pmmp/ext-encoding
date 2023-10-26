--TEST--
Test that new ByteBuffer() works correctly
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("abc");
var_dump($buffer->toString());

$buffer = new ByteBuffer();
var_dump($buffer->toString());

?>
--EXPECT--
string(3) "abc"
string(0) ""
