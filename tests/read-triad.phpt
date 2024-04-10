--TEST--
Test that reading triads works correctly
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\Types;

$buffer = new ByteBuffer("\xff\x00\x00");

var_dump(Types::readSignedTriadBE($buffer));

$buffer->setReadOffset(0);
var_dump(Types::readSignedTriadLE($buffer));

$buffer->setReadOffset(0);
var_dump(Types::readUnsignedTriadBE($buffer));

$buffer->setReadOffset(0);
var_dump(Types::readUnsignedTriadLE($buffer));

?>
--EXPECT--
int(-65536)
int(255)
int(16711680)
int(255)

