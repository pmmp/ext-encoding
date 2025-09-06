<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80100
 */

namespace pmmp\encoding;

/**
 * @strict-properties
 */
final class ByteBufferWriter{
	/**
	 * Constructs a new ByteBufferWriter.
     * The provided string will be written at the start of the buffer as if readByteArray() was called.
	 */
	public function __construct(string $prefix = ""){}

	/**
	 * Returns a string containing the written bytes.
	 * Reserved memory is not included.
	 */
	public function getData() : string{}

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
	 * Returns the current internal write offset (the position
	 * from which the next write operation will start).
	 */
	public function getOffset() : int{}

	/**
	 * Sets the internal write offset to the given value.
	 * The offset must be within the bounds of the buffer
	 * (0 <= offset <= reserved length).
	 *
	 * @throws \ValueError if the offset is out of bounds
	 */
	public function setOffset(int $offset) : void{}

	/**
	 * Returns the total number of bytes written.
	 * This will always be less than or equal to the reserved length.
	 */
	public function getUsedLength() : int{}

	/**
	 * Returns the number of bytes reserved by the ByteBuffer.
	 * This value may be larger than the number of written bytes, as
	 * some memory may be preallocated to avoid reallocations.
	 */
	public function getReservedLength() : int{}

	/**
	 * Increases buffer capacity to the given value, if the capacity
	 * is less than this amount. Useful to avoid extra reallocations
	 * during large write operations when the needed capacity of the
	 * buffer is known in advance.
	 */
	public function reserve(int $length) : void{}

	/**
	 * Truncates the internal buffer to only the written part,
	 * discarding any unused reserved memory.
	 */
	public function trim() : void{}

	/**
	 * Clears all data from the buffer. The memory used is retained
	 * as reserved memory.
	 */
	public function clear() : void{}

	public function __serialize() : array{}

	public function __unserialize(array $data) : void{}

	public function __debugInfo() : array{}
}
