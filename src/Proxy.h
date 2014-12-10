#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "NetworkManager.h"

#include <map>

class Proxy {
public:
    static Proxy *getProxy();

    static NetworkManager *getNetwork();

    static Profile &getProfile(string_t);

    static void addProfile(string_t, Profile);

    Proxy();

    ~Proxy();

private:
    static Proxy *instance;
    NetworkManager *network;
    std::map<string_t, Profile> uuids;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
