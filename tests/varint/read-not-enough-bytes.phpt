--TEST--
read(Un)SignedVar(Int|Long)() must correctly handle not being given enough bytes
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\DataDecodeException;
use pmmp\encoding\VarInt;

function test(\Closure $function) : void{
	$buffer = new ByteBufferReader("\x80");
	try{
		$function($buffer);
	}catch(DataDecodeException $e){
		echo "no offset, not enough bytes: " . $e->getMessage() . "\n";
	}

	$buffer = new ByteBufferReader("\x00\x00\x00\x00\x80");
	try{
		$buffer->setOffset(4);
		$function($buffer);
	}catch(DataDecodeException $e){
		echo "offset valid, not enough bytes: " . $e->getMessage() . "\n";
	}

	echo "\n";
}

test(VarInt::readUnsignedInt(...));
test(VarInt::readSignedInt(...));
test(VarInt::readUnsignedLong(...));
test(VarInt::readSignedLong(...));

?>
--EXPECT--
no offset, not enough bytes: No bytes left in buffer
offset valid, not enough bytes: No bytes left in buffer

no offset, not enough bytes: No bytes left in buffer
offset valid, not enough bytes: No bytes left in buffer

no offset, not enough bytes: No bytes left in buffer
offset valid, not enough bytes: No bytes left in buffer

no offset, not enough bytes: No bytes left in buffer
offset valid, not enough bytes: No bytes left in buffer
