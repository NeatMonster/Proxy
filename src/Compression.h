#ifndef __Proxy__Compression__
#define __Proxy__Compression__

#include "Types.h"

class Compression {
public:
    struct CompressionException : public std::runtime_error {
    public:
        CompressionException(string_t s) : std::runtime_error(s) {}
    };

    typedef std::pair<ubyte_t*, size_t> result_t;

    static result_t deflateZLib(ubyte_t*, size_t);

    static result_t inflateZlib(ubyte_t*, size_t);
};

#endif /* defined(__Proxy__Compression__) */
