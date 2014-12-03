#ifndef __Proxy__PacketHandler__
#define __Proxy__PacketHandler__

class PacketHandshake;
class PacketPing;
class PacketRequest;
class PlayerConnection;

class PacketHandler {
public:
    PacketHandler(PlayerConnection*);

    void handleHandshake(PacketHandshake*);

    void handleRequest(PacketRequest*);

    void handlePing(PacketPing*);

private:
    PlayerConnection *connect;
};

#endif /* defined(__Proxy__PacketHandler__) */
