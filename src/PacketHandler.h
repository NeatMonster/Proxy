#ifndef __Proxy__PacketHandler__
#define __Proxy__PacketHandler__

class PacketHandshake;
class PacketRequest;
class PlayerConnection;

class PacketHandler {
public:
    PacketHandler(PlayerConnection*);

    void handleHandshake(PacketHandshake*);

    void handleRequest(PacketRequest*);

private:
    PlayerConnection *connect;
};

#endif /* defined(__Proxy__PacketHandler__) */
