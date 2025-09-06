--TEST--
Test that ByteBufferReader::__debugInfo() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$buffer = new ByteBufferReader("looooooooooooong short");
var_dump($buffer);
?>
--EXPECTF--
object(pmmp\encoding\ByteBufferReader)#%d (2) {
  ["buffer"]=>
  string(22) "looooooooooooong short"
  ["offset"]=>
  int(0)
}
