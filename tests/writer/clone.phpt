--TEST--
Test that cloning ByteBufferWriter works correctly
--DESCRIPTION--
byte_buffer->used wasn't being copied during clones, leading to the cloned object appearing to have an empty buffer
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;

$buffer = new ByteBufferWriter("Some Data");
$buffer2 = clone $buffer;
var_dump($buffer);
var_dump($buffer2);

var_dump($buffer->getData());
var_dump($buffer2->getData());

?>
--EXPECTF--
object(pmmp\encoding\ByteBufferWriter)#%d (2) {
  ["buffer"]=>
  string(9) "Some Data"
  ["offset"]=>
  int(9)
}
object(pmmp\encoding\ByteBufferWriter)#%d (2) {
  ["buffer"]=>
  string(9) "Some Data"
  ["offset"]=>
  int(9)
}
string(9) "Some Data"
string(9) "Some Data"
