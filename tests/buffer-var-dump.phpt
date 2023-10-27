--TEST--
Test that ByteBuffer::__debugInfo() doesn't read out-of-bounds when dumping internal buffer
--DESCRIPTION--
The debuginfo handler wasn't accounting for the possibility that the used portion of the internal buffer may be smaller than the allocated capacity.
In some cases, this could lead to segfaults due to reading out-of-bounds.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer();
$buffer->writeByteArray("looooooooooooong");
$buffer->writeByteArray(" short"); //this will result in a buffer larger than the contents as the previous size will be doubled
var_dump($buffer);
?>
--EXPECTF--
object(pmmp\encoding\ByteBuffer)#%d (2) {
  ["buffer"]=>
  string(22) "looooooooooooong short"
  ["offset"]=>
  int(22)
}
