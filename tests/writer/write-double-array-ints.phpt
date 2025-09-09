--TEST--
write(Float|Double)Array() should accept IS_LONG values
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\LE;
use pmmp\encoding\BE;

$var = [1, 2.0, 3];

$writer = new ByteBufferWriter();
LE::writeDoubleArray($writer, $var);
var_dump(bin2hex($writer->getData()));

$writer = new ByteBufferWriter();
BE::writeFloatArray($writer, $var);
var_dump(bin2hex($writer->getData()));
?>
--EXPECTF--
string(48) "000000000000f03f00000000000000400000000000000840"
string(24) "3f8000004000000040400000"
