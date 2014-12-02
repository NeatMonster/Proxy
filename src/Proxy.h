#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "PlayerConnection.h"

#include <vector>

class Proxy {
public:
    Proxy();

    ~Proxy();

private:
    std::vector<PlayerConnection*> connects;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
