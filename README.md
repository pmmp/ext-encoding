# ext-encoding
This extension implements a `ByteBuffer` class, a high-performance alternative for [`pocketmine/binaryutils`](https://github.com/pmmp/BinaryUtils).

## :warning: This extension is EXPERIMENTAL

There is a high likelihood that the extension's classes may crash or behave incorrectly.
Do not use this extension for anything you don't want to get horribly corrupted.

## API
A recent IDE stub can usually be found in our [custom stubs repository](https://github.com/pmmp/phpstorm-stubs/blob/fork/encoding/encoding.php).

#### :warning: The API design is not yet finalized. Everything is still subject to change prior to the 1.0.0 release.

### FAQs about API design
#### Why are there SO MANY functions? Why not just accept something like `bool $signed, ByteOrder $order` parameters?

Runtime parameters would mean that these hot encoding paths would need to branch to decide how to encode everything. Branching is slow, so we want to avoid that.

Internally, we actually only have a handful number of functions (defined in `Serializers.h`), which use C++ templates to inject type, signedness, and byte order arguments.
This allows the compiler to expand these templates into optimised branchless native functions for each `(type, signed, byte order)` combination.
So, the C++ side is actually quite clean.

Now about the PHP part. Parsing arguments in PHP is slow, because we have to do dynamic type verification everywhere. This adds a significant extra amount of potential branching per argument.
Since PHP doesn't have anything akin to C++ templates (or generics more generally), the only option is to generate a separate PHP function for every combination of `(type, signed, byte order)`.
This way, the knowledge about signedness and byte-order is baked into the function name (basically a worse version of C++ templates).

The downside of this is that we can't use `.stub.php` files to generate arginfo, so the IDE stubs have to be generated from the extension using [extension-stub-generator](https://github.com/pmmp/extension-stub-generator).
Also, you'll probably need eye bleach after seeing the [macros that generate the function matrix](https://github.com/pmmp/ext-encoding/blob/bfcc8243f1037d37efea53444dc17c11bd2d47df/classes/Types.cpp#L246-L365).

However, considering how critical binary data handling is to performance in PocketMine-MP, this is a trade absolutely worth making.

#### Why static methods instead of `ByteBuffer` instance methods?

Two reasons:
- As described above, the static `read`/`write` methods can't be generated using `.stub.php` files. If we put the generated functions in `ByteBuffer` itself, we'd be unable to use a `.stub.php` file to define the rest of its non-generated API.
- For reversible fixed-size types in particular: Since byte order is decided by the very first character you type (and you have to import `BE` or `LE` to use those functions), you won't get caught out by IDE auto completions and accidentally mix byte order without noticing. This is a lesson hard learned from years of `BinaryStream` use, where a sneaky misplaced `L` can make or break a packet.

I may yet change the design again, anyway, so don't get too comfortable with it :^)

#### Why fully specify `Signed` or `Unsigned` in every function name? Why not just have e.g. `readInt()` and `readUint()`?

I still might change this yet.

This library's first users will be people moving from `BinaryStream`, where the API is infamous for being inconsistent about signedness when not specified (https://github.com/pmmp/BinaryUtils/issues/15). For example, `getShort()` is unsigned, and `getInt()` is signed.

I felt that it was better to be verbose to force developers to think about whether to use a signed or an unsigned type when migrating old code.

## Real-world performance tests
- [`pocketmine/nbt`](https://github.com/pmmp/NBT) was tested with release 0.2.1, and showed 1.5x read and 2x write performance with some basic synthetic tests.

## Why is `BinaryStream` and generally `pocketmine/binaryutils` so slow?
- [VarInt encode/decode](#varint-encodedecode)
- [`pack()` and `unpack()`](#pack-and-unpack)
- [Linear buffer reallocations](#linear-buffer-reallocations)
- [Array-of-type](#foreach-array-of-type)

### VarInt encode/decode
VarInts are heavily used by the Bedrock protocol, the theory being to reduce the size of integer types on the wire.
This format is borrowed from [protobuf](https://developers.google.com/protocol-buffers/docs/encoding).

Implemented in PHP, it's abysmally slow, due to repeated calls to `chr()` and `ord()` in a loop, as well as needing workarounds for PHP's lack of logical rightshift.

Compared to `BinaryStream`, `ByteBuffer` offers a performance improvement of 5-10x (depending on the size of the value and other conditions, YMMV) with both signed and unsigned varints.

This is extremely significant for PocketMine-MP due to the number of hot paths affected by such a performance gain (e.g. chunk encoding will benefit significantly).

### `pack()` and `unpack()`
Under a profiler, it becomes obvious that PHP's `pack()` and `unpack()` functions are abysmally slow, due to the cost of parsing the formatting code argument.
This parsing takes over 90% of the time spent in `pack()` and `unpack()`.
This overhead can be easily avoided when the types of data used are known in advance.

This extension implements specialized functions for writing big and little endian byte/short/int/long/float/double. 
Depending on the type and other factors, these functions typically show a 3-4x performance improvement compared to `BinaryStream`.

### Linear buffer reallocations
`BinaryStream` and similar PHP-land byte-buffer implementations often use strings and use the `.=` concatenation operator.
This is problematic, because the entire string will be reallocated every time something is appended to it.
While this isn't a big issue for small buffers, the performance of writing to large buffers progressively degrades.

`ByteBuffer` uses exponential scaling (factor of 2) to minimize buffer reallocations at the cost of potentially wasting some memory.
This means that the internal buffer size is doubled when the buffer runs out of space.

### Array-of-type
All the above problems contribute to this one, in addition to:
- Extra function call overhead
- Dealing with PHP `HashTable` structures is generally slow (a problem not solved by this extension currently)

`VarInt::readSignedIntArray()`, for example, was found to be over 50 times faster in simple tests than a loop calling `BinaryStream::getVarInt()` when dealing with an array of 10k elements.
The most obvious cases where this will benefit PocketMine-MP are in `LevelChunkPacket` encoding, and plugins using `ClientboundMapItemDataPacket` could also benefit from it.

In the future it'll probably make sense to add PHP wrappers for native array-of-type (e.g. `IntArray`, `LongArray` etc) so that we can avoid the performance and memory usage penalties
of dealing with large primitive arrays at runtime.
