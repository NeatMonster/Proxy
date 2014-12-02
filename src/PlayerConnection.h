#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ClientSocket.h"

#include <thread>

class PlayerConnection {
public:
    PlayerConnection(ClientSocket*);

    ~PlayerConnection();

    void runRead();

    void runWrite();

private:
    ClientSocket *socket;
    std::thread readThread;
    std::thread writeThread;
};

#endif /* defined(__Proxy__PlayerConnection__) */
