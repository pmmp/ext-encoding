# ext-encoding
High-performance specialized replacements for PHP's pack() and unpack() functions

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
