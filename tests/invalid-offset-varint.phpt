--TEST--
read(Un)SignedVar(Int|Long)() must correctly handle being given weird offset values
--EXTENSIONS--
encoding
--FILE--
<?php

function test(\Closure $c) : void{
	try{
		$offset = 4;
		$c("\x00", $offset);
	}catch(ValueError $e){
		echo "offset larger than buffer size: " . $e->getMessage() . "\n";
	}

	try{
		$offset = -4;
		$c("\x00", $offset);
	}catch(ValueError $e){
		echo "offset negative: " . $e->getMessage() . "\n";
	}

	try{
		$offset = "1";
		$c("\x00", $offset);
	}catch(TypeError $e){
		echo "wrong type offset: " . $e->getMessage() . "\n";
	}
	
	echo "\n";
}

test(\Closure::fromCallable('readUnsignedVarInt'));
test(\Closure::fromCallable('readSignedVarInt'));
test(\Closure::fromCallable('readUnsignedVarLong'));
test(\Closure::fromCallable('readSignedVarLong'));

?>
--EXPECT--
offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given

offset larger than buffer size: $offset must be less than the length (1) of the input string, 4 given
offset negative: $offset expects at least 0, -4 given
wrong type offset: $offset expects int|null, string given
