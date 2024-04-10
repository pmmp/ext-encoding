--TEST--
Test that writing triads works as expected
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\Types;

$buffer = new ByteBuffer();
Types::writeSignedTriadBE($buffer, -65536);
var_dump($buffer->toString() === "\xff\x00\x00");

$buffer = new ByteBuffer();
Types::writeSignedTriadLE($buffer, -65536);
var_dump($buffer->toString() === "\x00\x00\xff");

$buffer = new ByteBuffer();
Types::writeUnsignedTriadBE($buffer, -65536);
var_dump($buffer->toString() === "\xff\x00\x00");

$buffer = new ByteBuffer();
Types::writeUnsignedTriadLE($buffer, -65536);
var_dump($buffer->toString() === "\x00\x00\xff");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
