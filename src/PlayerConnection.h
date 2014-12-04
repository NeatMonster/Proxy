#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ByteBuffer.h"
#include "ClientSocket.h"
#include "Packet.h"
#include "PacketHandler.h"

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

    void runClient();

    void runServer();

    void sendToClient(Packet*);

    void sendToClient(ubyte_t*, varint_t);

    void sendToServer(Packet*);

    void sendToServer(ubyte_t*, varint_t);

    void connect();

    void disconnect(string_t);

private:
    ClientSocket *cSocket;
    ClientSocket *sSocket;
    std::thread cThread;
    std::thread sThread;
    ByteBuffer cReadBuffer;
    ByteBuffer sReadBuffer;
    ByteBuffer cWriteBuffer;
    ByteBuffer sWriteBuffer;
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
