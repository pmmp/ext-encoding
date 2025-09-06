--TEST--
Test symmetry of all encode/decode methods and their array variants
--FILE--
<?php

use pmmp\encoding\Byte;
use pmmp\encoding\VarInt;

require __DIR__ . '/type-samples.inc';
require __DIR__ . '/symmetry-tests.inc';

foreach(EndianInts::cases() as $case){
	$samples = $case->getSamples();
	testFullSymmetry("big endian " . $case->name, $samples, ...$case->getMethods(true));
	testFullSymmetry("little endian " . $case->name, $samples, ...$case->getMethods(false));
}

testSingleSymmetry("unsigned byte", [0, 1, 127, 128, 254, 255], Byte::readUnsigned(...), Byte::writeUnsigned(...));
testSingleSymmetry("signed byte", [-128, -1, 0, 1, 126, 127], Byte::readSigned(...), Byte::writeSigned(...));

foreach(FloatSamples::cases() as $case){
	$samples = [-1.5, -1.0, 0.0, 1.0, 1.5]; //TODO: float bounds are hard to test
	testFullSymmetry("big endian " . $case->name, $samples, ...$case->getMethods(true));
	testFullSymmetry("little endian " . $case->name, $samples, ...$case->getMethods(false));
}

testFullSymmetry("varint", EndianInts::UNSIGNED_INT->getSamples(), VarInt::readUnsignedInt(...), VarInt::writeUnsignedInt(...), VarInt::readUnsignedIntArray(...), VarInt::writeUnsignedIntArray(...));
testFullSymmetry("varint zigzag", EndianInts::SIGNED_INT->getSamples(), VarInt::readSignedInt(...), VarInt::writeSignedInt(...), VarInt::readSignedIntArray(...), VarInt::writeSignedIntArray(...));
testFullSymmetry("varlong", EndianInts::SIGNED_LONG->getSamples(), VarInt::readUnsignedLong(...), VarInt::writeUnsignedLong(...), VarInt::readUnsignedLongArray(...), VarInt::writeUnsignedLongArray(...));
testFullSymmetry("varlong zigzag", EndianInts::SIGNED_LONG->getSamples(), VarInt::readSignedLong(...), VarInt::writeSignedLong(...), VarInt::readSignedLongArray(...), VarInt::writeSignedLongArray(...));
?>
--EXPECT--
########## TEST big endian UNSIGNED_SHORT ##########
--- big endian UNSIGNED_SHORT single read symmetry ---
(0): match = YES
(1): match = YES
(65534): match = YES
(65535): match = YES
--- big endian UNSIGNED_SHORT array symmetry: match = YES
--- big endian UNSIGNED_SHORT array vs single symmetry: match = YES
--- big endian UNSIGNED_SHORT single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_SHORT ##########

########## TEST little endian UNSIGNED_SHORT ##########
--- little endian UNSIGNED_SHORT single read symmetry ---
(0): match = YES
(1): match = YES
(65534): match = YES
(65535): match = YES
--- little endian UNSIGNED_SHORT array symmetry: match = YES
--- little endian UNSIGNED_SHORT array vs single symmetry: match = YES
--- little endian UNSIGNED_SHORT single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_SHORT ##########

########## TEST big endian SIGNED_SHORT ##########
--- big endian SIGNED_SHORT single read symmetry ---
(-32768): match = YES
(-32767): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(32766): match = YES
(32767): match = YES
--- big endian SIGNED_SHORT array symmetry: match = YES
--- big endian SIGNED_SHORT array vs single symmetry: match = YES
--- big endian SIGNED_SHORT single vs array symmetry: match = YES
########## END TEST big endian SIGNED_SHORT ##########

########## TEST little endian SIGNED_SHORT ##########
--- little endian SIGNED_SHORT single read symmetry ---
(-32768): match = YES
(-32767): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(32766): match = YES
(32767): match = YES
--- little endian SIGNED_SHORT array symmetry: match = YES
--- little endian SIGNED_SHORT array vs single symmetry: match = YES
--- little endian SIGNED_SHORT single vs array symmetry: match = YES
########## END TEST little endian SIGNED_SHORT ##########

########## TEST big endian UNSIGNED_TRIAD ##########
--- big endian UNSIGNED_TRIAD single read symmetry ---
(0): match = YES
(1): match = YES
(16777214): match = YES
(16777215): match = YES
--- big endian UNSIGNED_TRIAD array symmetry: match = YES
--- big endian UNSIGNED_TRIAD array vs single symmetry: match = YES
--- big endian UNSIGNED_TRIAD single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_TRIAD ##########

########## TEST little endian UNSIGNED_TRIAD ##########
--- little endian UNSIGNED_TRIAD single read symmetry ---
(0): match = YES
(1): match = YES
(16777214): match = YES
(16777215): match = YES
--- little endian UNSIGNED_TRIAD array symmetry: match = YES
--- little endian UNSIGNED_TRIAD array vs single symmetry: match = YES
--- little endian UNSIGNED_TRIAD single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_TRIAD ##########

########## TEST big endian SIGNED_TRIAD ##########
--- big endian SIGNED_TRIAD single read symmetry ---
(-8388608): match = YES
(-8388607): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(8388606): match = YES
(8388607): match = YES
--- big endian SIGNED_TRIAD array symmetry: match = YES
--- big endian SIGNED_TRIAD array vs single symmetry: match = YES
--- big endian SIGNED_TRIAD single vs array symmetry: match = YES
########## END TEST big endian SIGNED_TRIAD ##########

########## TEST little endian SIGNED_TRIAD ##########
--- little endian SIGNED_TRIAD single read symmetry ---
(-8388608): match = YES
(-8388607): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(8388606): match = YES
(8388607): match = YES
--- little endian SIGNED_TRIAD array symmetry: match = YES
--- little endian SIGNED_TRIAD array vs single symmetry: match = YES
--- little endian SIGNED_TRIAD single vs array symmetry: match = YES
########## END TEST little endian SIGNED_TRIAD ##########

########## TEST big endian UNSIGNED_INT ##########
--- big endian UNSIGNED_INT single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- big endian UNSIGNED_INT array symmetry: match = YES
--- big endian UNSIGNED_INT array vs single symmetry: match = YES
--- big endian UNSIGNED_INT single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_INT ##########

########## TEST little endian UNSIGNED_INT ##########
--- little endian UNSIGNED_INT single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- little endian UNSIGNED_INT array symmetry: match = YES
--- little endian UNSIGNED_INT array vs single symmetry: match = YES
--- little endian UNSIGNED_INT single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_INT ##########

########## TEST big endian SIGNED_INT ##########
--- big endian SIGNED_INT single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- big endian SIGNED_INT array symmetry: match = YES
--- big endian SIGNED_INT array vs single symmetry: match = YES
--- big endian SIGNED_INT single vs array symmetry: match = YES
########## END TEST big endian SIGNED_INT ##########

########## TEST little endian SIGNED_INT ##########
--- little endian SIGNED_INT single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- little endian SIGNED_INT array symmetry: match = YES
--- little endian SIGNED_INT array vs single symmetry: match = YES
--- little endian SIGNED_INT single vs array symmetry: match = YES
########## END TEST little endian SIGNED_INT ##########

########## TEST big endian SIGNED_LONG ##########
--- big endian SIGNED_LONG single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- big endian SIGNED_LONG array symmetry: match = YES
--- big endian SIGNED_LONG array vs single symmetry: match = YES
--- big endian SIGNED_LONG single vs array symmetry: match = YES
########## END TEST big endian SIGNED_LONG ##########

########## TEST little endian SIGNED_LONG ##########
--- little endian SIGNED_LONG single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- little endian SIGNED_LONG array symmetry: match = YES
--- little endian SIGNED_LONG array vs single symmetry: match = YES
--- little endian SIGNED_LONG single vs array symmetry: match = YES
########## END TEST little endian SIGNED_LONG ##########

--- unsigned byte single read symmetry ---
(0): match = YES
(1): match = YES
(127): match = YES
(128): match = YES
(254): match = YES
(255): match = YES
--- signed byte single read symmetry ---
(-128): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(126): match = YES
(127): match = YES
########## TEST big endian FLOAT ##########
--- big endian FLOAT single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- big endian FLOAT array symmetry: match = YES
--- big endian FLOAT array vs single symmetry: match = YES
--- big endian FLOAT single vs array symmetry: match = YES
########## END TEST big endian FLOAT ##########

########## TEST little endian FLOAT ##########
--- little endian FLOAT single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- little endian FLOAT array symmetry: match = YES
--- little endian FLOAT array vs single symmetry: match = YES
--- little endian FLOAT single vs array symmetry: match = YES
########## END TEST little endian FLOAT ##########

########## TEST big endian DOUBLE ##########
--- big endian DOUBLE single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- big endian DOUBLE array symmetry: match = YES
--- big endian DOUBLE array vs single symmetry: match = YES
--- big endian DOUBLE single vs array symmetry: match = YES
########## END TEST big endian DOUBLE ##########

########## TEST little endian DOUBLE ##########
--- little endian DOUBLE single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- little endian DOUBLE array symmetry: match = YES
--- little endian DOUBLE array vs single symmetry: match = YES
--- little endian DOUBLE single vs array symmetry: match = YES
########## END TEST little endian DOUBLE ##########

########## TEST varint ##########
--- varint single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- varint array symmetry: match = YES
--- varint array vs single symmetry: match = YES
--- varint single vs array symmetry: match = YES
########## END TEST varint ##########

########## TEST varint zigzag ##########
--- varint zigzag single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- varint zigzag array symmetry: match = YES
--- varint zigzag array vs single symmetry: match = YES
--- varint zigzag single vs array symmetry: match = YES
########## END TEST varint zigzag ##########

########## TEST varlong ##########
--- varlong single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- varlong array symmetry: match = YES
--- varlong array vs single symmetry: match = YES
--- varlong single vs array symmetry: match = YES
########## END TEST varlong ##########

########## TEST varlong zigzag ##########
--- varlong zigzag single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- varlong zigzag array symmetry: match = YES
--- varlong zigzag array vs single symmetry: match = YES
--- varlong zigzag single vs array symmetry: match = YES
########## END TEST varlong zigzag ##########
