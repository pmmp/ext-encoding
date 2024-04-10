--TEST--
Test that reading triads works correctly
--DESCRIPTION--
Triads require special implementation due to not being a power-of-two size. This opens avenues for extra bugs that must be tested for.
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\BE;
use pmmp\encoding\LE;

$buffer = new ByteBuffer("\xff\x00\x00");

var_dump(BE::readSignedTriad($buffer));

$buffer->setReadOffset(0);
var_dump(LE::readSignedTriad($buffer));

$buffer->setReadOffset(0);
var_dump(BE::readUnsignedTriad($buffer));

$buffer->setReadOffset(0);
var_dump(LE::readUnsignedTriad($buffer));

?>
--EXPECT--
int(-65536)
int(255)
int(16711680)
int(255)

