<?php

use pmmp\encoding\Types;

return [
    [Types::readUnsignedShortLE(...), "\xff\xff"],
    [Types::readSignedShortLE(...), "\xff\xff"],
    [Types::readUnsignedShortBE(...), "\xff\xff"],
    [Types::readSignedShortBE(...), "\xff\xff"],

    [Types::readUnsignedIntLE(...), "\xff\xff\xff\xff"],
    [Types::readSignedIntLE(...), "\xff\xff\xff\xff"],
    [Types::readFloatLE(...), "\x11\xff\xff\x11"],
    [Types::readUnsignedIntBE(...), "\xff\xff\xff\xff"],
    [Types::readSignedIntBE(...), "\xff\xff\xff\xff"],
    [Types::readFloatBE(...), "\x11\xff\xff\x11"],

    [Types::readSignedLongLE(...), "\xff\xff\xff\xff\xff\xff\xff\xff"],
    [Types::readSignedLongBE(...), "\xff\xff\xff\xff\xff\xff\xff\xff"],
    [Types::readDoubleLE(...), "\x11\xff\xff\x11\x11\xff\xff\x11"],
    [Types::readDoubleBE(...), "\x11\xff\xff\x11\x11\xff\xff\x11"],

    [Types::readUnsignedTriadBE(...), "\xff\xee\xdd"],
    [Types::readUnsignedTriadLE(...), "\xdd\xee\xff"],
    [Types::readSignedTriadBE(...), "\xff\xee\xdd"],
    [Types::readSignedTriadLE(...), "\xdd\xee\xff"],
];
