<?php

/** @generate-class-entries */

namespace pmmp\encoding;

/**
 * @strict-properties
 */
final class ByteBufferReader{
	/**
	 * Constructs a new ByteBufferReader.
	 * Offset will be initialized to 0.
	 */
	public function __construct(string $data){}

	/**
	 * Returns the string (byte array) that the reader is reading.
	 */
	public function getData() : string{}

	/**
	 * Reads $length raw bytes from the buffer at the current offset.
	 * The internal offset will be updated by this operation.
     *
     * @throws DataDecodeException if there are not enough bytes available
	 */
	public function readByteArray(int $length) : string{}

	/**
	 * Returns the current internal read offset (the position
	 * from which the next read operation will start).
	 */
	public function getOffset() : int{}

	/**
	 * Sets the internal read offset to the given value.
	 * The offset must be within the bounds of the buffer
	 * (0 <= offset <= used length).
	 *
	 * @throws \ValueError if the offset is out of bounds
	 */
	public function setOffset(int $offset) : void{}

	public function __serialize() : array{}

	public function __unserialize(array $data) : void{}

	public function __debugInfo() : array{}
}
