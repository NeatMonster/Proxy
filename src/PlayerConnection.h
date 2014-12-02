#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "PacketFactory.h"

#include <atomic>
#include <thread>
#include <map>

class PlayerConnection {
    friend class PacketHandler;

public:
    PlayerConnection(ClientSocket*);

    ~PlayerConnection();

    void close();

    bool isClosed();

    void runRead();

    void runWrite();

private:
    static PacketFactory factories[4];

    ClientSocket *socket;
    std::thread readThread;
    std::thread writeThread;
    ByteBuffer readBuffer;
    ByteBuffer writeBuffer;
    PacketHandler *handler;
    std::atomic<bool> closed;
    std::atomic<PacketFactory::Phase> phase;
};

#endif /* defined(__Proxy__PlayerConnection__) */
