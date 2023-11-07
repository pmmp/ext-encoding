<?php

/** @generate-class-entries */

namespace pmmp\encoding;

/**
 * @strict-properties
 */
abstract class BaseByteBuffer{
	public function __construct(string $buffer = ""){}

	public function toString() : string{}

	public function readByteArray(int $length) : string{}

	public function writeByteArray(string $value) : void{}

	public function getOffset() : int{}

	public function setOffset(int $offset) : void{}

	public function getReserved() : int{}

	public function reserve(int $length) : void{}

	public function trim() : void{}

	public function rewind() : void{}

	public function getUnreadLength() : int{}

	public function __serialize() : array{}

	public function __unserialize(array $data) : void{}

	public function __debugInfo() : array{}
}
