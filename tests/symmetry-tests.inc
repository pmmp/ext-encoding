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
	echo "--- $label array symmetry ---\n";

	$writer = new ByteBufferWriter();
	$writeArrayMethod($writer, $samples);

	$reader = new ByteBufferReader($writer->getData());
	$decoded = $readArrayMethod($reader, count($samples));
	echo "(" . print_r($samples, true) . "): match = " . ($samples === $decoded ? "YES" : "NO") . "\n";
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
	echo "########## END TEST " . $label . " ##########\n\n";
}
