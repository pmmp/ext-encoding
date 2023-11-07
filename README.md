# ext-encoding
This extension implements a `ByteBuffer` class, a high-performance alternative for [`pocketmine/binaryutils`](https://github.com/pmmp/BinaryUtils).

## :warning: This extension is EXPERIMENTAL

There is a high likelihood that the extension's classes may crash or behave incorrectly.
Do not use this extension for anything you don't want to get horribly corrupted.

## API
A recent IDE stub can usually be found in our [custom stubs repository](https://github.com/pmmp/phpstorm-stubs/blob/fork/encoding/encoding.php).

#### :warning: The API design is not yet finalized. Everything is still subject to change prior to the 1.0.0 release.

## Real-world performance tests
- [`pocketmine/nbt`](https://github.com/pmmp/NBT) was tested with release 0.2.1, and showed 1.5x read and 2x write performance with some basic synthetic tests.

## Performance considerations
### VarInts
VarInts are heavily used by the Bedrock protocol, the theory being to reduce the size of integer types on the wire.
This format is borrowed from [protobuf](https://developers.google.com/protocol-buffers/docs/encoding).

Implemented in PHP, it's abysmally slow, due to repeated calls to `chr()` and `ord()` in a loop, as well as needing workarounds for PHP's lack of logical rightshift.

Compared to `BinaryStream`, `ByteBuffer` offers a performance improvement of 5-10x (depending on the size of the value and other conditions, YMMV) with both signed and unsigned varints.

This is extremely significant for PocketMine-MP due to the number of hot paths affected by such a performance gain (e.g. chunk encoding will benefit significantly).

### Fixed size types
Under a profiler, it becomes obvious that PHP's `pack()` and `unpack()` functions are abysmally slow, due to the cost of parsing the formatting code argument.
This parsing takes over 90% of the time spent in `pack()` and `unpack()`.
This overhead can be easily avoided when the types of data used are known in advance.

This extension implements specialized functions for writing big and little endian byte/short/int/long/float/double. 
Depending on the type and other factors, these functions typically show a 3-4x performance improvement compared to `BinaryStream`.

### Linear memory allocations
`BinaryStream` and similar PHP-land byte-buffer implementations often use strings and use the `.=` concatenation operator.
This is problematic, because the entire string will be reallocated every time something is appended to it.
While this isn't a big issue for small buffers, the performance of writing to large buffers progressively degrades.

`ByteBuffer` uses exponential scaling (factor of 2) to minimize buffer reallocations at the cost of potentially wasting some memory.
This means that the internal buffer size is doubled when the buffer runs out of space.
