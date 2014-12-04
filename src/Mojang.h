#ifndef __Proxy__Mojang__
#define __Proxy__Mojang__

#include "Types.h"

#include <stdexcept>
#include <thread>

class Mojang {
public:
    struct Profile {
        struct Property {
            string_t name;
            string_t value;
            string_t signature;
        };

        string_t uuid;
        string_t name;
        std::vector<Property> properties;
    };

    struct SSLException : public std::runtime_error {
        SSLException(string_t s) : std::runtime_error(s) {}
    };

    static Profile *authentificate(string_t, string_t, ubytes_t, ubytes_t);

private:
    static const string_t certificates;
};

typedef Mojang::Profile Profile;

#endif /* defined(__Proxy__Mojang__) */
