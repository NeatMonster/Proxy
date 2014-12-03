#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "PacketHandler.h"
#include "ServerPacket.h"

#include <atomic>
#include <thread>
#include <map>

class PlayerConnection {
    friend class PacketHandler;

public:
    enum Phase {
        HANDSHAKE, STATUS, LOGIN, PLAY
    };

    PlayerConnection(ClientSocket*);

    ~PlayerConnection();

    void close();

    bool isClosed();

    void run();

    void sendPacket(ServerPacket*);

private:
    ClientSocket *socket;
    std::thread thread;
    ByteBuffer readBuffer;
    ByteBuffer writeBuffer;
    PacketHandler *handler;
    std::atomic<bool> closed;
    std::atomic<Phase> phase;
};

#endif /* defined(__Proxy__PlayerConnection__) */
