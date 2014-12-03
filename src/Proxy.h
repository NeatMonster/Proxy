#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "NetworkManager.h"

class Proxy {
public:
    Proxy();

    ~Proxy();

private:
    NetworkManager *network;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
