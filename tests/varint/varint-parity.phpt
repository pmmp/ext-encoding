--TEST--
Test that varint encoding matches some known binary values
--DESCRIPTION--
We already verify that all the encoders and decoders are symmetric in another test.
This test verifies parity with BinaryUtils using some known values.
--FILE--
<?php

use pmmp\encoding\ByteBufferWriter;
use pmmp\encoding\VarInt;

$sample32 = require __DIR__ . '/varint-binary-samples.inc';

function check(string $label, string $actual, string $expected) : void{
    echo "case $label: match = " . ($actual === hex2bin($expected) ? "YES" : "NO") . "\n";
}

echo "--- varint ---\n";
foreach($sample32 as $case => [$unsignedValue, $signedValue, $negativeSignedValue, $unsignedBinary, $signedBinary, $negativeSignedBinary]){
    $writer = new ByteBufferWriter();
    VarInt::writeUnsignedInt($writer, $unsignedValue);
    check("$case (unsigned)", $writer->getData(), $unsignedBinary);

    $writer = new ByteBufferWriter();
    VarInt::writeSignedInt($writer, $signedValue);
    check("$case (signed, positive)", $writer->getData(), $signedBinary);

    $writer = new ByteBufferWriter();
    VarInt::writeSignedInt($writer, $negativeSignedValue);
    check("$case (signed, negative)", $writer->getData(), $negativeSignedBinary);
}

$sample64 = require __DIR__ . '/varlong-binary-samples.inc';

echo "--- varlong ---\n";
foreach($sample64 as $case => [$unsignedValue, $signedValue, $negativeSignedValue, $unsignedBinary, $signedBinary, $negativeSignedBinary]){
    $writer = new ByteBufferWriter();
    VarInt::writeUnsignedLong($writer, $unsignedValue);
    check("$case (unsigned)", $writer->getData(), $unsignedBinary);

    $writer = new ByteBufferWriter();
    VarInt::writeSignedLong($writer, $signedValue);
    check("$case (signed, positive)", $writer->getData(), $signedBinary);

    $writer = new ByteBufferWriter();
    VarInt::writeSignedLong($writer, $negativeSignedValue);
    check("$case (signed, negative)", $writer->getData(), $negativeSignedBinary);
}
echo "--- end ---\n";
?>
--EXPECT--
--- varint ---
case 0 (unsigned): match = YES
case 0 (signed, positive): match = YES
case 0 (signed, negative): match = YES
case 1 (unsigned): match = YES
case 1 (signed, positive): match = YES
case 1 (signed, negative): match = YES
case 2 (unsigned): match = YES
case 2 (signed, positive): match = YES
case 2 (signed, negative): match = YES
case 3 (unsigned): match = YES
case 3 (signed, positive): match = YES
case 3 (signed, negative): match = YES
case 4 (unsigned): match = YES
case 4 (signed, positive): match = YES
case 4 (signed, negative): match = YES
--- varlong ---
case 0 (unsigned): match = YES
case 0 (signed, positive): match = YES
case 0 (signed, negative): match = YES
case 1 (unsigned): match = YES
case 1 (signed, positive): match = YES
case 1 (signed, negative): match = YES
case 2 (unsigned): match = YES
case 2 (signed, positive): match = YES
case 2 (signed, negative): match = YES
case 3 (unsigned): match = YES
case 3 (signed, positive): match = YES
case 3 (signed, negative): match = YES
case 4 (unsigned): match = YES
case 4 (signed, positive): match = YES
case 4 (signed, negative): match = YES
case 5 (unsigned): match = YES
case 5 (signed, positive): match = YES
case 5 (signed, negative): match = YES
case 6 (unsigned): match = YES
case 6 (signed, positive): match = YES
case 6 (signed, negative): match = YES
case 7 (unsigned): match = YES
case 7 (signed, positive): match = YES
case 7 (signed, negative): match = YES
case 8 (unsigned): match = YES
case 8 (signed, positive): match = YES
case 8 (signed, negative): match = YES
case 9 (unsigned): match = YES
case 9 (signed, positive): match = YES
case 9 (signed, negative): match = YES
--- end ---
