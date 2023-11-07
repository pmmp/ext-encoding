<?php

/** @generate-class-entries */

namespace pmmp\encoding;

/**
 * @strict-properties
 */
abstract class BaseByteBuffer{
	public function __construct(string $buffer = ""){}

	/**
	 * Returns a string containing the initialized bytes.
	 * Reserved memory is not included.
	 */
	public function toString() : string{}

	/**
	 * Reads $length raw bytes from the buffer at the current offset.
	 * The internal offset will be updated by this operation.
	 */
	public function readByteArray(int $length) : string{}

	/**
	 * Writes the given bytes to the buffer at the current offset.
	 * The internal offset will be updated by this operation.
	 *
	 * If the current buffer size is not big enough to add the given
	 * bytes, the buffer will be resized to either 2x its current size,
	 * or the actual size of the result, whichever is larger. This
	 * ensures the lowest number of reallocations.
	 */
	public function writeByteArray(string $value) : void{}

	/**
	 * Returns the current internal offset (the position from
	 * which the next read/write operation will start).
	 */
	public function getOffset() : int{}

	/**
	 * Sets the internal offset to the given value.
	 * The offset must be within the intialized part of the buffer.
	 */
	public function setOffset(int $offset) : void{}

	/**
	 * Returns the number of bytes reserved by the ByteBuffer.
	 * This value may be larger than the number of readable bytes, as
	 * some memory may be preallocated to avoid reallocations.
	 */
	public function getReserved() : int{}

	/**
	 * Increases buffer capacity to the given value, if the capacity
	 * is less than this amount. Useful to avoid extra reallocations
	 * during large write operations when the needed capacity of the
	 * buffer is known in advance.
	 */
	public function reserve(int $length) : void{}

	/**
	 * Truncates the internal buffer to only the initialized part,
	 * discarding any unused reserved memory.
	 */
	public function trim() : void{}

	/**
	 * Sets the internal offset to the start of the buffer.
	 */
	public function rewind() : void{}

	/**
	 * Returns the number of bytes available to read after the
	 * current offset.
	 */
	public function getUnreadLength() : int{}

	public function __serialize() : array{}

	public function __unserialize(array $data) : void{}

	public function __debugInfo() : array{}
}
