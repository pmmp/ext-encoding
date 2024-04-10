--TEST--
Test that ByteBuffer read/write methods behave the same as their pack/unpack equivalents
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\Types;

$map = [
    "C" => [Types::readUnsignedByte(...), Types::writeUnsignedByte(...), [0, 127, 128, 255]],
    "c" => [Types::readSignedByte(...), Types::writeSignedByte(...), [-128, -1, 0, 1, 127]],

    //signed short doesn't have any pack() equivalent
    "n" => [Types::readUnsignedShortBE(...), Types::writeUnsignedShortBE(...), [0, 1, 32767, 32768, 65535]],
    "v" => [Types::readUnsignedShortLE(...), Types::writeUnsignedShortLE(...), [0, 1, 32767, 32768, 65535]],

    //signed long doesn't have any pack() equivalent
    "N" => [Types::readUnsignedIntBE(...), Types::writeUnsignedIntBE(...), [0, 1, 2147483647, 2147483648, 4294967295]],
    "V" => [Types::readUnsignedIntLE(...), Types::writeUnsignedIntLE(...), [0, 1, 2147483647, 2147483648, 4294967295]],

    //these codes are supposed to be unsigned int64, but there's no such thing in PHP
    //the negative bounds must be written weirdly here due to weirdness in PHP parser
    "J" => [Types::readSignedLongBE(...), Types::writeSignedLongBE(...), [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],
    "P" => [Types::readSignedLongLE(...), Types::writeSignedLongLE(...), [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],

    "G" => [Types::readFloatBE(...), Types::writeFloatBE(...), [-1.0, 0.0, 1.0]],
    "g" => [Types::readFloatLE(...), Types::writeFloatLE(...), [-1.0, 0.0, 1.0]],

    "E" => [Types::readDoubleBE(...), Types::writeDoubleBE(...), [-1.0, 0.0, 1.0]],
    "e" => [Types::readDoubleLE(...), Types::writeDoubleLE(...), [-1.0, 0.0, 1.0]],
];

foreach($map as $packCode => [$readFunc, $writeFunc, $testValues]){
    echo "--- Testing equivalents for pack code \"$packCode\" with " . count($testValues) . " samples ---\n";
    foreach($testValues as $value){
        $expectedBytes = pack($packCode, $value);

        $buffer = new ByteBuffer();
        $writeFunc($buffer, $value);

        if($expectedBytes !== $buffer->toString()){
            echo "Mismatch \"$packCode\" write: " . bin2hex($expectedBytes) . " " . bin2hex($buffer->toString()) . "\n";
        }

        $buffer = new ByteBuffer($expectedBytes);
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
