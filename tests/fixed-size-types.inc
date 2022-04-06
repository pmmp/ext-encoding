<?php

return [
	'readUnsignedShortLE' => "\xff\xff",
	'readSignedShortLE' => "\xff\xff",
	'readUnsignedShortBE' => "\xff\xff",
	'readSignedShortBE' => "\xff\xff",

	'readUnsignedIntLE' => "\xff\xff\xff\xff",
	'readSignedIntLE' => "\xff\xff\xff\xff",
	'readFloatLE' => "\x11\xff\xff\x11",
	'readUnsignedIntBE' => "\xff\xff\xff\xff",
	'readSignedIntBE' => "\xff\xff\xff\xff",
	'readFloatBE' => "\x11\xff\xff\x11",

	'readSignedLongLE' => "\xff\xff\xff\xff\xff\xff\xff\xff",
	'readSignedLongBE' => "\xff\xff\xff\xff\xff\xff\xff\xff",
	'readDoubleLE' => "\x11\xff\xff\x11\x11\xff\xff\x11",
	'readDoubleBE' => "\x11\xff\xff\x11\x11\xff\xff\x11",
];
