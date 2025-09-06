--TEST--
Test that ByteBuffer serializes and unserializes correctly
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;

$buffer = new ByteBufferWriter("hello world");
$buffer2 = unserialize(serialize($buffer));

var_dump($buffer, $buffer2);

?>
--EXPECTF--
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
