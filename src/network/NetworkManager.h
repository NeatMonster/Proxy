#ifndef __Proxy__NetworkManager__
#define __Proxy__NetworkManager__

#include "PlayerConnection.h"
#include "ServerSocket.h"

#include <thread>
#include <vector>

class NetworkManager {
public:
    NetworkManager();

    ~NetworkManager();

    bool start();

    bool stop();

    void cleanup();

private:
    bool running;
    std::thread thread;
    ServerSocket *socket;
    std::vector<PlayerConnection*> connects;

    void run();
};

#endif /* defined(__Proxy__NetworkManager__) */
