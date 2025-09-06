--TEST--
Test symmetry of all encode/decode methods and their array variants
--FILE--
<?php

use pmmp\encoding\Byte;
use pmmp\encoding\VarInt;

require __DIR__ . '/type-samples.inc';
require __DIR__ . '/symmetry-tests.inc';

//these have no array equivalents
echo "########## TEST unsigned byte ##########\n";
testSingleSymmetry([0, 1, 127, 128, 254, 255], Byte::readUnsigned(...), Byte::writeUnsigned(...));
echo "########## END TEST unsigned byte ##########\n\n";

echo "########## TEST signed byte ##########\n";
testSingleSymmetry([-128, -1, 0, 1, 126, 127], Byte::readSigned(...), Byte::writeSigned(...));
echo "########## END TEST signed byte ##########\n\n";

foreach(EndianInts::cases() as $case){
	$samples = $case->getSamples();
	testFullSymmetry("big endian " . $case->name, $samples, ...$case->getMethods(true));
	testFullSymmetry("little endian " . $case->name, $samples, ...$case->getMethods(false));
}



foreach(FloatSamples::cases() as $case){
	$samples = [-1.5, -1.0, 0.0, 1.0, 1.5]; //TODO: float bounds are hard to test
	testFullSymmetry("big endian " . $case->name, $samples, ...$case->getMethods(true));
	testFullSymmetry("little endian " . $case->name, $samples, ...$case->getMethods(false));
}

testFullSymmetry("varint", EndianInts::UNSIGNED_INT->getSamples(), VarInt::readUnsignedInt(...), VarInt::writeUnsignedInt(...), VarInt::readUnsignedIntArray(...), VarInt::writeUnsignedIntArray(...));
testFullSymmetry("varint zigzag", EndianInts::SIGNED_INT->getSamples(), VarInt::readSignedInt(...), VarInt::writeSignedInt(...), VarInt::readSignedIntArray(...), VarInt::writeSignedIntArray(...));
testFullSymmetry("varlong", EndianInts::UNSIGNED_LONG->getSamples(), VarInt::readUnsignedLong(...), VarInt::writeUnsignedLong(...), VarInt::readUnsignedLongArray(...), VarInt::writeUnsignedLongArray(...));
testFullSymmetry("varlong zigzag", EndianInts::SIGNED_LONG->getSamples(), VarInt::readSignedLong(...), VarInt::writeSignedLong(...), VarInt::readSignedLongArray(...), VarInt::writeSignedLongArray(...));
?>
--EXPECT--
########## TEST unsigned byte ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(127): match = YES
(128): match = YES
(254): match = YES
(255): match = YES
########## END TEST unsigned byte ##########

########## TEST signed byte ##########
--- single read symmetry ---
(-128): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(126): match = YES
(127): match = YES
########## END TEST signed byte ##########

########## TEST big endian UNSIGNED_SHORT ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(65534): match = YES
(65535): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_SHORT ##########

########## TEST little endian UNSIGNED_SHORT ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(65534): match = YES
(65535): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_SHORT ##########

########## TEST big endian SIGNED_SHORT ##########
--- single read symmetry ---
(-32768): match = YES
(-32767): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(32766): match = YES
(32767): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian SIGNED_SHORT ##########

########## TEST little endian SIGNED_SHORT ##########
--- single read symmetry ---
(-32768): match = YES
(-32767): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(32766): match = YES
(32767): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian SIGNED_SHORT ##########

########## TEST big endian UNSIGNED_TRIAD ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(16777214): match = YES
(16777215): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_TRIAD ##########

########## TEST little endian UNSIGNED_TRIAD ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(16777214): match = YES
(16777215): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_TRIAD ##########

########## TEST big endian SIGNED_TRIAD ##########
--- single read symmetry ---
(-8388608): match = YES
(-8388607): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(8388606): match = YES
(8388607): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian SIGNED_TRIAD ##########

########## TEST little endian SIGNED_TRIAD ##########
--- single read symmetry ---
(-8388608): match = YES
(-8388607): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(8388606): match = YES
(8388607): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian SIGNED_TRIAD ##########

########## TEST big endian UNSIGNED_INT ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_INT ##########

########## TEST little endian UNSIGNED_INT ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_INT ##########

########## TEST big endian SIGNED_INT ##########
--- single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian SIGNED_INT ##########

########## TEST little endian SIGNED_INT ##########
--- single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian SIGNED_INT ##########

########## TEST big endian UNSIGNED_LONG ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian UNSIGNED_LONG ##########

########## TEST little endian UNSIGNED_LONG ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian UNSIGNED_LONG ##########

########## TEST big endian SIGNED_LONG ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian SIGNED_LONG ##########

########## TEST little endian SIGNED_LONG ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian SIGNED_LONG ##########

########## TEST big endian FLOAT ##########
--- single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian FLOAT ##########

########## TEST little endian FLOAT ##########
--- single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian FLOAT ##########

########## TEST big endian DOUBLE ##########
--- single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST big endian DOUBLE ##########

########## TEST little endian DOUBLE ##########
--- single read symmetry ---
(-1.5): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(1.5): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST little endian DOUBLE ##########

########## TEST varint ##########
--- single read symmetry ---
(0): match = YES
(1): match = YES
(4294967294): match = YES
(4294967295): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST varint ##########

########## TEST varint zigzag ##########
--- single read symmetry ---
(-2147483648): match = YES
(-2147483647): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(2147483646): match = YES
(2147483647): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST varint zigzag ##########

########## TEST varlong ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST varlong ##########

########## TEST varlong zigzag ##########
--- single read symmetry ---
(-9223372036854775808): match = YES
(-9223372036854775807): match = YES
(-1): match = YES
(0): match = YES
(1): match = YES
(9223372036854775806): match = YES
(9223372036854775807): match = YES
--- array symmetry: match = YES
--- array vs single symmetry: match = YES
--- single vs array symmetry: match = YES
########## END TEST varlong zigzag ##########
