--TEST--
Test that ByteBufferReader::__debugInfo() works as expected
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$buffer = new ByteBufferReader("looooooooooooong short");
var_dump($buffer);
$buffer = new ByteBufferReader(substr("looooooooooooong short" . chr(mt_rand(0, 255)), 0, -1)); //make sure the string is refcounted (not interned)
var_dump($buffer);
?>
--EXPECTF--
object(pmmp\encoding\ByteBufferReader)#%d (2) {
  ["buffer"]=>
  string(22) "looooooooooooong short"
  ["offset"]=>
  int(0)
}
object(pmmp\encoding\ByteBufferReader)#%d (2) {
  ["buffer"]=>
  string(22) "looooooooooooong short"
  ["offset"]=>
  int(0)
}
