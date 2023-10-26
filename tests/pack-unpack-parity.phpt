--TEST--
Test that ByteBuffer read/write methods behave the same as their pack/unpack equivalents
--FILE--
<?php

use pmmp\encoding\ByteBuffer;

$map = [
    "C" => [fn(ByteBuffer $b) => [$b->readUnsignedByte(...), $b->writeUnsignedByte(...)], [0, 127, 128, 255]],
    "c" => [fn(ByteBuffer $b) => [$b->readSignedByte(...), $b->writeSignedByte(...)], [-128, -1, 0, 1, 127]],

    //signed short doesn't have any pack() equivalent
    "n" => [fn(ByteBuffer $b) => [$b->readUnsignedShortBE(...), $b->writeUnsignedShortBE(...)], [0, 1, 32767, 32768, 65535]],
    "v" => [fn(ByteBuffer $b) => [$b->readUnsignedShortLE(...), $b->writeUnsignedShortLE(...)], [0, 1, 32767, 32768, 65535]],

    //signed long doesn't have any pack() equivalent
    "N" => [fn(ByteBuffer $b) => [$b->readUnsignedIntBE(...), $b->writeUnsignedIntBE(...)], [0, 1, 2147483647, 2147483648, 4294967295]],
    "V" => [fn(ByteBuffer $b) => [$b->readUnsignedIntLE(...), $b->writeUnsignedIntLE(...)], [0, 1, 2147483647, 2147483648, 4294967295]],

    //these codes are supposed to be unsigned int64, but there's no such thing in PHP
    //the negative bounds must be written weirdly here due to weirdness in PHP parser
    "J" => [fn(ByteBuffer $b) => [$b->readSignedLongBE(...), $b->writeSignedLongBE(...)], [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],
    "P" => [fn(ByteBuffer $b) => [$b->readSignedLongLE(...), $b->writeSignedLongLE(...)], [-9223372036854775807-1, -1, 0, 1, 9223372036854775807]],

    "G" => [fn(ByteBuffer $b) => [$b->readFloatBE(...), $b->writeFloatBE(...)], [-1.0, 0.0, 1.0]],
    "g" => [fn(ByteBuffer $b) => [$b->readFloatLE(...), $b->writeFloatLE(...)], [-1.0, 0.0, 1.0]],

    "E" => [fn(ByteBuffer $b) => [$b->readDoubleBE(...), $b->writeDoubleBE(...)], [-1.0, 0.0, 1.0]],
    "e" => [fn(ByteBuffer $b) => [$b->readDoubleLE(...), $b->writeDoubleLE(...)], [-1.0, 0.0, 1.0]],
];

foreach($map as $packCode => [$getRWFuncs, $testValues]){
    echo "--- Testing equivalents for pack code \"$packCode\" with " . count($testValues) . " samples ---\n";
    foreach($testValues as $value){
        $expectedBytes = pack($packCode, $value);

        $buffer = new ByteBuffer();
        $writeFunc = $getRWFuncs($buffer)[1];
        $writeFunc($value);

        if($expectedBytes !== $buffer->toString()){
            echo "Mismatch \"$packCode\" write: " . bin2hex($expectedBytes) . " " . bin2hex($buffer->toString()) . "\n";
        }

        $buffer = new ByteBuffer($expectedBytes);
        $readFunc = $getRWFuncs($buffer)[0];
        $decodedValue = $readFunc();

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
