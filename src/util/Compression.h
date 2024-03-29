#ifndef __Proxy__Compression__
#define __Proxy__Compression__

#include "Types.h"

#include <stdexcept>

class Compression {
public:
    struct CompressionException : public std::runtime_error {
    public:
        CompressionException(string_t s) : std::runtime_error(s) {}
    };

    static size_t deflateZLib(ubyte_t*, size_t, ubyte_t*, size_t);

    static size_t inflateZlib(ubyte_t*, size_t, ubyte_t*, size_t);
};

#endif /* defined(__Proxy__Compression__) */
