<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80100
 */

namespace pmmp\encoding;

/**
 * @strict-properties
 */
final class ByteBufferReader{
	/** @genstubs-expose-comment-block
	 * Constructs a new ByteBufferReader.
	 * Offset will be initialized to 0.
	 */
	public function __construct(string $data){}

	/** @genstubs-expose-comment-block
	 * Returns the string (byte array) that the reader is reading.
	 */
	public function getData() : string{}

	/** @genstubs-expose-comment-block
	 * Reads $length raw bytes from the buffer at the current offset.
	 * The internal offset will be updated by this operation.
	 *
	 * @throws DataDecodeException if there are not enough bytes available
	 */
	public function readByteArray(int $length) : string{}

	/** @genstubs-expose-comment-block
	 * Returns the current internal read offset (the position
	 * from which the next read operation will start).
	 */
	public function getOffset() : int{}

	/** @genstubs-expose-comment-block
	 * Sets the internal read offset to the given value.
	 * The offset must be within the bounds of the buffer
	 * (0 <= offset <= used length).
	 *
	 * @throws \ValueError if the offset is out of bounds
	 */
	public function setOffset(int $offset) : void{}

	/** @genstubs-expose-comment-block
	 * Returns the number of bytes available to read after the
	 * current offset.
	 */
	public function getUnreadLength() : int{}

	public function __serialize() : array{}

	public function __unserialize(array $data) : void{}

	public function __debugInfo() : array{}
}
