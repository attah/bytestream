# Bytestream

An attempt at vaguely Erlang-like binary handling for C++

For easy, readable, construction and deconstruction of binaries - with built-in endianness handling.

Used in https://github.com/attah/harbour-seaprint for IPP and (m)DNS encoding and decoding.

(Mis-)used in https://github.com/attah/ppm2pwg for the PWG raster compression algorithm.

See [tests](tests/test.cpp) for examples.

## Codable

`__attribute__((packed))` on steroids, implemented with Bytestream.

Used in https://github.com/attah/ppm2pwg for file/page headers.
