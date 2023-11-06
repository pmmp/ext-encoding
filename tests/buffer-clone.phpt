--TEST--
Test that cloning ByteBuffer works correctly
--DESCRIPTION--
byte_buffer->used wasn't being copied during clones, leading to the cloned object appearing to have an empty buffer
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("Some Data");
$buffer2 = clone $buffer;
var_dump($buffer);
var_dump($buffer2);

var_dump($buffer->toString());
var_dump($buffer2->toString());

?>
--EXPECTF--
object(pmmp\encoding\ByteBuffer)#%d (2) {
  ["buffer"]=>
  string(9) "Some Data"
  ["offset"]=>
  int(0)
}
object(pmmp\encoding\ByteBuffer)#%d (2) {
  ["buffer"]=>
  string(9) "Some Data"
  ["offset"]=>
  int(0)
}
string(9) "Some Data"
string(9) "Some Data"
