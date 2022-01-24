--TEST--
read(Un)SignedVar(Int|Long)() must correctly handle not being given enough bytes
--EXTENSIONS--
encoding
--FILE--
<?php

function test(\Closure $c) : void{
	try{
		$c("\x80");
	}catch(DataDecodeException $e){
		echo "no offset, not enough bytes: " . $e->getMessage() . "\n";
	}

	try{
		$offset = 4;
		$c("\x00\x00\x00\x00\x80", $offset);
	}catch(DataDecodeException $e){
		echo "offset valid, not enough bytes: " . $e->getMessage() . "\n";
	}

	echo "\n";
}

test(\Closure::fromCallable('readUnsignedVarInt'));
test(\Closure::fromCallable('readSignedVarInt'));
test(\Closure::fromCallable('readUnsignedVarLong'));
test(\Closure::fromCallable('readSignedVarLong'));

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
