#ifndef __Proxy__Mojang__
#define __Proxy__Mojang__

#include "Profile.h"
#include "Types.h"

#include <stdexcept>
#include <thread>

class Mojang {
public:
    struct SSLException : public std::runtime_error {
        SSLException(string_t s) : std::runtime_error(s) {}
    };

    static bool authentificate(Profile*, string_t, ubytes_t, ubytes_t);

private:
    static const string_t certificates;
};

#endif /* defined(__Proxy__Mojang__) */
