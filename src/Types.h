#ifndef __Proxy__Types__
#define __Proxy__Types__

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

typedef std::uint8_t ubyte_t;
typedef std::uint16_t ushort_t;
typedef std::uint32_t uint_t;
typedef std::uint64_t ulong_t;
typedef std::string string_t;

struct InvalidArgumentException : std::runtime_error {
    InvalidArgumentException(string_t s) : std::runtime_error(s) {}
};

#endif /* defined(__Proxy__Types__) */
