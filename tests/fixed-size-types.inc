<?php

use pmmp\encoding\BE;
use pmmp\encoding\LE;

return [
    [LE::readUnsignedShort(...), "\xff\xff"],
    [LE::readSignedShort(...), "\xff\xff"],
    [BE::readUnsignedShort(...), "\xff\xff"],
    [BE::readSignedShort(...), "\xff\xff"],

    [LE::readUnsignedInt(...), "\xff\xff\xff\xff"],
    [LE::readSignedInt(...), "\xff\xff\xff\xff"],
    [LE::readFloat(...), "\x11\xff\xff\x11"],
    [BE::readUnsignedInt(...), "\xff\xff\xff\xff"],
    [BE::readSignedInt(...), "\xff\xff\xff\xff"],
    [BE::readFloat(...), "\x11\xff\xff\x11"],

    [LE::readSignedLong(...), "\xff\xff\xff\xff\xff\xff\xff\xff"],
    [BE::readSignedLong(...), "\xff\xff\xff\xff\xff\xff\xff\xff"],
    [LE::readDouble(...), "\x11\xff\xff\x11\x11\xff\xff\x11"],
    [BE::readDouble(...), "\x11\xff\xff\x11\x11\xff\xff\x11"],

    [BE::readUnsignedTriad(...), "\xff\xee\xdd"],
    [LE::readUnsignedTriad(...), "\xdd\xee\xff"],
    [BE::readSignedTriad(...), "\xff\xee\xdd"],
    [LE::readSignedTriad(...), "\xdd\xee\xff"],
];
