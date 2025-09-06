<?php

use pmmp\encoding\ByteBufferReader;
use pmmp\encoding\ByteBufferWriter;

/**
 * @phpstan-template TValue
 * @phpstan-param TValue[] $samples
 * @phpstan-param \Closure(ByteBufferReader):TValue $readMethod
 * @phpstan-param \Closure(ByteBufferWriter,TValue):void $writeMethod
 */
function testSingleSymmetry(string $label, array $samples, \Closure $readMethod, \Closure $writeMethod) : void{
	echo "--- $label single read symmetry ---\n";
	foreach($samples as $sample){
		$writer = new ByteBufferWriter();
		$writeMethod($writer, $sample);

		$reader = new ByteBufferReader($writer->getData());
		$decoded = $readMethod($reader);

		echo "($sample): match = " . ($sample === $decoded ? "YES" : "NO") . "\n";
	}
}

/**
 * @phpstan-template TValue
 * @phpstan-param TValue[] $samples
 * @phpstan-param \Closure(ByteBufferReader,int):array<TValue> $readArrayMethod
 * @phpstan-param \Closure(ByteBufferWriter,array<TValue>):void $writeArrayMethod
 */
function testArraySymmetry(string $label, array $samples, \Closure $readArrayMethod, \Closure $writeArrayMethod) : void{
	$writer = new ByteBufferWriter();
	$writeArrayMethod($writer, $samples);

	$reader = new ByteBufferReader($writer->getData());
	$decoded = $readArrayMethod($reader, count($samples));
	echo "--- $label array symmetry: match = " . ($samples === $decoded ? "YES" : "NO") . "\n";
}

/**
 * @phpstan-template TValue
 * @phpstan-param TValue[] $samples
 * @phpstan-param \Closure(ByteBufferReader):TValue $readMethod
 * @phpstan-param \Closure(ByteBufferWriter,TValue):void $writeMethod
 * @phpstan-param \Closure(ByteBufferReader,int):array<TValue> $readArrayMethod
 * @phpstan-param \Closure(ByteBufferWriter,array<TValue>):void $writeArrayMethod
 */
function testArrayVsSingleSymmetry(string $label, array $samples, \Closure $readMethod, \Closure $writeMethod, \Closure $readArrayMethod, \Closure $writeArrayMethod) : void{
	$writer = new ByteBufferWriter();
	foreach($samples as $sample){
		$writeMethod($writer, $sample);
	}
	$reader = new ByteBufferReader($writer->getData());
	$decodedArray = $readArrayMethod($reader, count($samples));
	echo "--- $label array vs single symmetry: match = " . ($samples === $decodedArray ? "YES" : "NO") . "\n";

	$writer = new ByteBufferWriter();
	$writeArrayMethod($writer, $samples);

	$reader = new ByteBufferReader($writer->getData());
	$decodedSingles = [];
	for($i = 0, $size = count($samples); $i < $size; $i++){
		$decodedSingles[] = $readMethod($reader);
	}
	echo "--- $label single vs array symmetry: match = " . ($samples === $decodedSingles ? "YES" : "NO") . "\n";
}

/**
 * @phpstan-template TValue
 * @phpstan-param TValue[] $samples
 * @phpstan-param \Closure(ByteBufferReader):TValue $readMethod
 * @phpstan-param \Closure(ByteBufferWriter,TValue):void $writeMethod
 * @phpstan-param \Closure(ByteBufferReader,int):array<TValue> $readArrayMethod
 * @phpstan-param \Closure(ByteBufferWriter,array<TValue>):void $writeArrayMethod
 */
function testFullSymmetry(string $label, array $samples, \Closure $readMethod, \Closure $writeMethod, \Closure $readArrayMethod, \Closure $writeArrayMethod) : void{
	echo "########## TEST " . $label . " ##########\n";
	testSingleSymmetry($label, $samples, $readMethod, $writeMethod);
	testArraySymmetry($label, $samples, $readArrayMethod, $writeArrayMethod);
	testArrayVsSingleSymmetry($label, $samples, $readMethod, $writeMethod, $readArrayMethod, $writeArrayMethod);
	echo "########## END TEST " . $label . " ##########\n\n";
}
