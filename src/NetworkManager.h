#ifndef __Proxy__NetworkManager__
#define __Proxy__NetworkManager__

#include "PlayerConnection.h"

#include <thread>
#include <vector>

class NetworkManager {
public:
    NetworkManager();

    ~NetworkManager();

    std::vector<PlayerConnection*> getConnections();

private:
    std::thread thread;
    std::vector<PlayerConnection*> connects;

    void run();
};

#endif /* defined(__Proxy__NetworkManager__) */
