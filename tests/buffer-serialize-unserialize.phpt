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
object(pmmp\encoding\ByteBuffer)#%d (3) {
  ["buffer"]=>
  string(11) "hello world"
  ["read_offset"]=>
  int(0)
  ["write_offset"]=>
  int(11)
}
object(pmmp\encoding\ByteBuffer)#%d (3) {
  ["buffer"]=>
  string(11) "hello world"
  ["read_offset"]=>
  int(0)
  ["write_offset"]=>
  int(11)
}
