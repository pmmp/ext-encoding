--TEST--
ByteBufferReader::getData() must manage string refcounts properly
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;

$reader = new ByteBufferReader(str_repeat("a", 10));
debug_zval_dump($reader->getData());
debug_zval_dump($reader->getData());
debug_zval_dump($reader->getData());
?>
--EXPECT--
string(10) "aaaaaaaaaa" refcount(2)
string(10) "aaaaaaaaaa" refcount(2)
string(10) "aaaaaaaaaa" refcount(2)
