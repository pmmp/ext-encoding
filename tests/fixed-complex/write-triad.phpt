--TEST--
Test that writing triads works as expected
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\BE;
use pmmp\encoding\LE;

$buffer = new ByteBufferWriter();
BE::writeSignedTriad($buffer, -65536);
var_dump($buffer->getData() === "\xff\x00\x00");

$buffer = new ByteBufferWriter();
LE::writeSignedTriad($buffer, -65536);
var_dump($buffer->getData() === "\x00\x00\xff");

$buffer = new ByteBufferWriter();
BE::writeUnsignedTriad($buffer, -65536);
var_dump($buffer->getData() === "\xff\x00\x00");

$buffer = new ByteBufferWriter();
LE::writeUnsignedTriad($buffer, -65536);
var_dump($buffer->getData() === "\x00\x00\xff");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
