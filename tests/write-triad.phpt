--TEST--
Test that writing triads works as expected
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\BE;
use pmmp\encoding\LE;

$buffer = new ByteBuffer();
BE::writeSignedTriad($buffer, -65536);
var_dump($buffer->toString() === "\xff\x00\x00");

$buffer = new ByteBuffer();
LE::writeSignedTriad($buffer, -65536);
var_dump($buffer->toString() === "\x00\x00\xff");

$buffer = new ByteBuffer();
BE::writeUnsignedTriad($buffer, -65536);
var_dump($buffer->toString() === "\xff\x00\x00");

$buffer = new ByteBuffer();
LE::writeUnsignedTriad($buffer, -65536);
var_dump($buffer->toString() === "\x00\x00\xff");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
