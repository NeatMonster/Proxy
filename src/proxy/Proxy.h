#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "ConfigManager.h"
#include "NetworkManager.h"

#include <unordered_map>

class Proxy {
public:
    static Proxy *getProxy();

    static ConfigManager *getConfig();

    static NetworkManager *getNetwork();

    static Profile &getProfile(string_t);

    static void addProfile(string_t, Profile);

    Proxy();

    ~Proxy();

private:
    static Proxy *instance;
    ConfigManager *config;
    NetworkManager *network;
    std::unordered_map<string_t, Profile> uuids;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
