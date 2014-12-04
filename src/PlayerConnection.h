#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "PacketHandler.h"
#include "ServerPacket.h"

#include "polarssl/aes.h"

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

    void join();

    void close();

    bool isClosed();

    void run();

    void sendPacket(ServerPacket*);

    void disconnect(string_t);

private:
    ClientSocket *socket;
    std::thread thread;
    ByteBuffer readBuffer;
    ByteBuffer writeBuffer;
    PacketHandler *handler;
    std::atomic<bool> closed;
    Phase phase;
    bool encryption;
    bool compression;
    ubyte_t iv_enc[16];
    ubyte_t iv_dec[16];
    aes_context aes_enc;
    aes_context aes_dec;

    void setup(ubytes_t*);
};

#endif /* defined(__Proxy__PlayerConnection__) */
