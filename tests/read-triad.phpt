--TEST--
Test that reading triads works correctly
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$buffer = new ByteBuffer("\xff\x00\x00");

var_dump($buffer->readSignedTriadBE());

$buffer->setReadOffset(0);
var_dump($buffer->readSignedTriadLE());

$buffer->setReadOffset(0);
var_dump($buffer->readUnsignedTriadBE());

$buffer->setReadOffset(0);
var_dump($buffer->readUnsignedTriadLE());

?>
--EXPECT--
int(-65536)
int(255)
int(16711680)
int(255)

