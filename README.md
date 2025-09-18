# ext-encoding
This extension provides high-performance raw data encoding & decoding utilities for PHP.

It was designed to supersede [`pocketmine/binaryutils`](https://github.com/pmmp/BinaryUtils) and the painfully slow PHP functions [`pack()`](https://www.php.net/manual/en/function.pack.php) and [`unpack()`](https://www.php.net/manual/en/function.unpack.php).

## Real-world performance tests
- [`pocketmine/nbt`](https://github.com/pmmp/NBT) was tested with release 0.2.1, and showed 1.5x read and 2x write performance with some basic synthetic tests.

## API
A recent IDE stub can usually be found in our [custom stubs repository](https://github.com/pmmp/phpstorm-stubs/blob/fork/encoding/encoding.php).

> [!NOTE]
> Although `ext-encoding` provides similar functionality to `pocketmine/binaryutils`, it is *not* a drop-in replacement.
> Its API is completely different and incompatible.

The new API has been designed with the lessons learned from `pocketmine/binaryutils` in mind. Most notably:
- Readers and writers have fully separated APIs - no more accidentally writing while intending to read or vice versa
- Endian-reversible types are implemented in `LE::` and `BE::` static methods, which avoids accidentally using the wrong byte order
- All integer-accepting and returning functions explicitly state whether they work with `Signed` or `Unsigned` integers

## FAQs
### Why are `BinaryStream` and generally `pocketmine/binaryutils` so slow?
- [VarInt encode/decode](#varint-encodedecode)
- [`pack()` and `unpack()`](#pack-and-unpack)
- [Linear buffer reallocations](#linear-buffer-reallocations)
- [Array-of-type](#array-of-type)

#### VarInt encode/decode
VarInts are heavily used by the Bedrock protocol. This format is borrowed from [protobuf](https://developers.google.com/protocol-buffers/docs/encoding).

There's no fast way to implement them in pure PHP. They require repeated calls to `chr()` and `ord()` in a loop, as well as needing workarounds for PHP's lack of logical rightshift.

Compared to `BinaryStream`, this extension's `VarInt::` functions offer a performance improvement of 5-10x (depending on the size of the value and other conditions, YMMV) with both signed and unsigned varints.

This will significantly improve performance in PocketMine-MP when integrated. For example, chunk encoding will become significantly faster, and encoding & decoding of almost all packets will benefit too.

#### `pack()` and `unpack()`
PHP's [`pack()`](https://www.php.net/manual/en/function.pack.php) and [`unpack()`](https://www.php.net/manual/en/function.unpack.php) functions are abysmally slow.
Parsing the formatting code argument takes over 90% of the time spent in these functions.
This overhead can be easily avoided when the types of data used are known in advance.

This extension implements specialized functions for writing big and little endian byte/short/int/long/float/double. 
Depending on the type and other factors, these functions typically show a 3-4x performance improvement compared to `BinaryStream`.

#### Linear buffer reallocations
`BinaryStream` and similar PHP-land byte-buffer implementations often use strings and use the `.=` concatenation operator.
This is problematic, because the entire string will be reallocated every time something is appended to it.
While this isn't a big issue for small buffers, the performance of writing to large buffers progressively degrades.

`ByteBufferWriter` uses exponential scaling (factor of 2) to minimize buffer reallocations at the cost of potentially wasting some memory.
This means that the internal buffer size is doubled when the buffer runs out of space.

#### Array-of-type
All the above problems contribute to this one, in addition to:
- Extra function call overhead
- Dealing with PHP `HashTable` structures is generally slow (a problem not solved by this extension currently)

During testing in the 0.x phase, writing batches of varints in a single function call was found to be over 50 times faster in simple tests than a loop calling `BinaryStream::getVarInt()` when dealing with an array of 10k elements.
The most obvious cases where this will benefit PocketMine-MP are in `LevelChunkPacket` encoding, and plugins using `ClientboundMapItemDataPacket` could also benefit from it.

However, the extension currently doesn't offer any functions for array-of-type encoding and decoding. This is because there are unresolved questions about how they should work, and I don't want to be locked into a particular API before I figure out what makes the most sense.

Some things to consider are:

- Some places that could write batches of something (e.g. `int[]`) need to transform the values before writing. In this case, a `foreach` and repeated `write*()` often ends up being faster than allocating an `array` for the transformed values.
- Some places that could write array-of-type don't actually store their data in PHP `array`s to begin with, but rather in native arrays (e.g. `PalettedBlockArray`). Forcing those cases to create a slow PHP `array`, when we just turn it back into a native array anyway, doesn't make any sense.
- Most places that use PHP `array`s that could be written without transformation are already paying performance and memory penalties for using PHP `array`s in the first place (e.g. `IntArrayTag`). They'd probably benefit from thin wrappers around native fixed arrays, e.g. `IntArray`, `LongArray` etc.

### Why are there SO MANY functions? Why not just accept something like `bool $signed, ByteOrder $order` parameters?

Runtime parameters would mean that these hot encoding paths would need to branch to decide how to encode everything. Branching is slow, so we want to avoid that.

Internally, we only have a handful of functions (defined in `Serializers.h`), which use C++ templates to inject type, signedness, and byte order arguments.
The compiler expands these templates into optimised branchless native functions for each `(type, signed, byte order)` combination.

In addition, parsing arguments in PHP is slow, and since PHP doesn't have anything akin to C++ templates (or generics more generally), the only option to get compile-time knowledge of byte order and signedness is to bake them into the function name. There is a function for every combination of `(type, signed, byte order)`.

The downside of this is that we can't use `.stub.php` files to generate arginfo, so the IDE stubs have to be generated from the extension using [extension-stub-generator](https://github.com/pmmp/extension-stub-generator).
Also, you'll probably need eye bleach after seeing the [macros that generate the function matrix](https://github.com/pmmp/ext-encoding/blob/bfcc8243f1037d37efea53444dc17c11bd2d47df/classes/Types.cpp#L246-L365).

However, considering how critical binary data handling is to performance in PocketMine-MP, this is a trade absolutely worth making.

### Why static methods instead of `ByteBuffer(Reader|Writer)` instance methods?

Two reasons:
- As described above, the static `read`/`write` methods can't be generated using `.stub.php` files. If we put the generated functions in `ByteBufferReader`/`ByteBufferWriter`, we'd be unable to use a `.stub.php` file to define the rest of its non-generated API.
- I've made too many mistakes with byte order due to IDE auto complete. With this API design, byte order is decided by the very first character you type, so auto complete can't trip you up (and you have to import `BE` or `LE`).
- For the other classes (`VarInt`, `Byte`), they don't really *need* to be static, but it makes more sense for the API to be consistent.

### Why fully specify `Signed` or `Unsigned` in every function name? Why not just have e.g. `readInt()` and `readUint()`?

This library's first users will be people moving from `BinaryStream`, where the API is infamous for being inconsistent about signedness when not specified (https://github.com/pmmp/BinaryUtils/issues/15). For example, `getShort()` is unsigned, and `getInt()` is signed.

I felt that it was better to be verbose to force developers to think about whether to use a signed or an unsigned type when migrating old code.
