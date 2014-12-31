#ifndef __Proxy__PlayerConnection__
#define __Proxy__PlayerConnection__

#include "ClientSocket.h"
#include "Packet.h"
#include "PacketBuffer.h"
#include "PacketHandler.h"

#include "polarssl/aes.h"

#include <atomic>
#include <thread>

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

    string_t getName();

    void runClient();

    void runServer();

    void sendToClient(Packet*);

    void sendToClient(ubyte_t*, varint_t);

    void sendToClient(varint_t);

    void sendToServer(Packet*);

    void sendToServer(ubyte_t*, varint_t);

    void connect();

    void disconnect(string_t);

private:
    ClientSocket *cSocket;
    ClientSocket *sSocket;
    std::thread cThread;
    std::thread sThread;
    PacketBuffer cReadBuffer;
    PacketBuffer sReadBuffer;
    PacketBuffer cWriteBuffer;
    PacketBuffer sWriteBuffer;
    PacketBuffer inflateBuffer;
    PacketBuffer deflateBuffer;
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
