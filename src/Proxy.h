#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "NetworkManager.h"

class Proxy {
public:
    static Proxy *getProxy();

    static NetworkManager *getNetwork();

    Proxy();

    ~Proxy();

private:
    static Proxy *instance;
    NetworkManager *network;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
