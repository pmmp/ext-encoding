--TEST--
Test that ByteBuffer serializes and unserializes correctly
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("hello world");
$buffer2 = unserialize(serialize($buffer));

var_dump($buffer, $buffer2);

?>
--EXPECTF--
object(pmmp\encoding\ByteBuffer)#%d (2) {
  ["buffer"]=>
  string(9) "hello world"
  ["offset"]=>
  int(0)
}
object(pmmp\encoding\ByteBuffer)#%d (2) {
  ["buffer"]=>
  string(9) "hello world"
  ["offset"]=>
  int(0)
}
