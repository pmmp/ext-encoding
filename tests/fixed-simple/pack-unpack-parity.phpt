--TEST--
Test that ByteBuffer read/write methods behave the same as their pack/unpack equivalents
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\BE;
use pmmp\encoding\LE;
use pmmp\encoding\Byte;

$map = [
    "C" => [Byte::readUnsigned(...), Byte::writeUnsigned(...), [0, 127, 128, 255]],
    "c" => [Byte::readSigned(...), Byte::writeSigned(...), [-128, -1, 0, 1, 127]],

    //signed short doesn't have any pack() equivalent
    "n" => [BE::readUnsignedShort(...), BE::writeUnsignedShort(...), [0, 1, 32767, 32768, 65535]],
    "v" => [LE::readUnsignedShort(...), LE::writeUnsignedShort(...), [0, 1, 32767, 32768, 65535]],

    //signed long doesn't have any pack() equivalent
    "N" => [BE::readUnsignedInt(...), BE::writeUnsignedInt(...), [0, 1, 2147483647, 2147483648, 4294967295]],
    "V" => [LE::readUnsignedInt(...), LE::writeUnsignedInt(...), [0, 1, 2147483647, 2147483648, 4294967295]],

    //these codes are supposed to be unsigned int64, but there's no such thing in PHP
    //the negative bounds must be written weirdly here due to weirdness in PHP parser
    "J" => [BE::readSignedLong(...), BE::writeSignedLong(...), [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],
    "P" => [LE::readSignedLong(...), LE::writeSignedLong(...), [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],

    "G" => [BE::readFloat(...), BE::writeFloat(...), [-1.0, 0.0, 1.0]],
    "g" => [LE::readFloat(...), LE::writeFloat(...), [-1.0, 0.0, 1.0]],

    "E" => [BE::readDouble(...), BE::writeDouble(...), [-1.0, 0.0, 1.0]],
    "e" => [LE::readDouble(...), LE::writeDouble(...), [-1.0, 0.0, 1.0]],
];

foreach($map as $packCode => [$readFunc, $writeFunc, $testValues]){
    echo "--- Testing equivalents for pack code \"$packCode\" with " . count($testValues) . " samples ---\n";
    foreach($testValues as $value){
        $expectedBytes = pack($packCode, $value);

        $buffer = new ByteBufferWriter();
        $writeFunc($buffer, $value);

        if($expectedBytes !== $buffer->getData()){
            echo "Mismatch \"$packCode\" write: " . bin2hex($expectedBytes) . " " . bin2hex($buffer->getData()) . "\n";
        }

        $buffer = new ByteBufferReader($expectedBytes);
        $decodedValue = $readFunc($buffer);

        if($value !== $decodedValue){
            echo "Mismatch \"$packCode\" read: " . $value . " " . $decodedValue . "\n";
        }
    }
}
?>
--EXPECT--
--- Testing equivalents for pack code "C" with 4 samples ---
--- Testing equivalents for pack code "c" with 5 samples ---
--- Testing equivalents for pack code "n" with 5 samples ---
--- Testing equivalents for pack code "v" with 5 samples ---
--- Testing equivalents for pack code "N" with 5 samples ---
--- Testing equivalents for pack code "V" with 5 samples ---
--- Testing equivalents for pack code "J" with 5 samples ---
--- Testing equivalents for pack code "P" with 5 samples ---
--- Testing equivalents for pack code "G" with 3 samples ---
--- Testing equivalents for pack code "g" with 3 samples ---
--- Testing equivalents for pack code "E" with 3 samples ---
--- Testing equivalents for pack code "e" with 3 samples ---
