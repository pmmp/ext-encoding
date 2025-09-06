--TEST--
Test that ByteBufferReader serializes and unserializes correctly
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$buffer = new ByteBufferReader("hello world");
$buffer2 = unserialize(serialize($buffer));

var_dump($buffer, $buffer2);

?>
--EXPECTF--
object(pmmp\encoding\ByteBufferReader)#%d (2) {
  ["buffer"]=>
  string(11) "hello world"
  ["offset"]=>
  int(0)
}
object(pmmp\encoding\ByteBufferReader)#%d (2) {
  ["buffer"]=>
  string(11) "hello world"
  ["offset"]=>
  int(0)
}
