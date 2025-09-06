--TEST--
Test that new ByteBufferWriter() works correctly
--DESCRIPTION--
Currently, giving a string to the constructor of a writer behaves the same way as doing 
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;

$buffer = new ByteBufferWriter();
var_dump($buffer->getData());
$buffer->writeByteArray("hello world");
var_dump($buffer->getData());

$buffer = new ByteBufferWriter("hello world");
$buffer2 = new ByteBufferWriter();
$buffer2->writeByteArray("hello world");

//these should have the same buffer and offset
var_dump($buffer, $buffer2);

?>
--EXPECTF--
string(0) ""
string(11) "hello world"
object(pmmp\encoding\ByteBufferWriter)#%d (2) {
  ["buffer"]=>
  string(11) "hello world"
  ["offset"]=>
  int(11)
}
object(pmmp\encoding\ByteBufferWriter)#%d (2) {
  ["buffer"]=>
  string(11) "hello world"
  ["offset"]=>
  int(11)
}
