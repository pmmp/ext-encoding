--TEST--
read(Un)SignedVar(Int|Long)() must correctly handle not being given enough bytes
--EXTENSIONS--
encoding
--FILE--
<?php

use pmmp\encoding\ByteBuffer;
use pmmp\encoding\DataDecodeException;

function test(string $function) : void{
	$buffer = new ByteBuffer("\x80");
	try{
		$buffer->$function();
	}catch(DataDecodeException $e){
		echo "no offset, not enough bytes: " . $e->getMessage() . "\n";
	}

	$buffer = new ByteBuffer("\x00\x00\x00\x00\x80");
	try{
		$offset = 4;
		$buffer->$function($offset);
	}catch(DataDecodeException $e){
		echo "offset valid, not enough bytes: " . $e->getMessage() . "\n";
	}

	echo "\n";
}

test('readUnsignedVarInt');
test('readSignedVarInt');
test('readUnsignedVarLong');
test('readSignedVarLong');

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
