#include "Profile.h"

Profile::Profile(string_t name) : name(name) {}

string_t Profile::getUUID() {
    return uuid;
}

string_t Profile::getName() {
    return name;
}

std::vector<Profile::Property> Profile::getProperties() {
    return properties;
}
