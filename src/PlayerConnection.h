#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ByteBuffer.h"
#include "ClientSocket.h"

#include <atomic>
#include <thread>

class PlayerConnection {
public:
    //enum Phase {
    //    HANDSHAKE, STATUS, LOGIN, PLAY
    //};

    PlayerConnection(ClientSocket*);

    ~PlayerConnection();

    void close();

    bool isClosed();

    void runRead();

    void runWrite();

private:
    ClientSocket *socket;
    std::thread readThread;
    std::thread writeThread;
    ByteBuffer readBuffer;
    ByteBuffer writeBuffer;
    std::atomic<bool> closed;
    //std::atomic<Phase> phase;
};

#endif /* defined(__Proxy__PlayerConnection__) */
