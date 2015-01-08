#ifndef __Proxy__Profile__
#define __Proxy__Profile__

#include "Types.h"

class Profile {
    friend class Mojang;

public:
    struct Property {
        string_t name;
        string_t value;
        bool isSigned;
        string_t signature;
    };

    Profile(string_t);

    string_t getUUID();

    string_t getName();

    std::vector<Property> getProperties();

private:
    string_t uuid;
    string_t name;
    std::vector<Property> properties;
};

#endif /* defined(__Proxy__Profile__) */
