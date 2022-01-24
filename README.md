# ext-encoding
High-performance specialized replacements for PHP's pack() and unpack() functions

## VarInts
VarInts are heavily used by the Bedrock protocol, the theory being to reduce the size of integer types on the wire.
This format is borrowed from [protobuf](https://developers.google.com/protocol-buffers/docs/encoding).

Implemented in PHP, it's abysmally slow, due to repeated calls to `chr()` and `ord()` in a loop, as well as needing workarounds for PHP's lack of logical rightshift.

Implemented in this extension, we see performance gains of around 10x compared to `Binary::writeVarInt()` and around 8x compared to `Binary::writeUnsignedVarInt()`.
This is extremely significant due to the number of hot paths affected by such a performance gain (e.g. chunk encoding will benefit significantly).

## Fixed size types
Under a profiler, it becomes obvious that PHP's `pack()` and `unpack()` functions are horribly slow due to the format code parsing.
This is a problem for PocketMine-MP because these functions are used in hot paths to encode packets ([pmmp/BinaryUtils](https://github.com/pmmp/BinaryUtils)).

This extension implements specialized replacements for these functions, such as:
- `readShortLE()`
- `readShortBE()`
- `readIntLE()`
- `readIntBE()`

and various others.

In synthetic benchmarks, it's shown that these functions are around 3x faster than `unpack()`.
When taking into account the overhead of `Binary` class and the possibility of implementing a native `Binary` class directly, the performance difference is closer to 7x faster.

## TODO
- Implement `Binary` as an extension class.
- Implement `BinaryStream` as an extension class (will elide 2 PHP function calls in most cases, saving a bunch of time).
