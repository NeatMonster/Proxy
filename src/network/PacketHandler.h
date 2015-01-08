#ifndef __Proxy__PacketHandler__
#define __Proxy__PacketHandler__

#include "Mojang.h"
#include "Types.h"

class PacketEncryptionResponse;
class PacketHandshake;
class PacketJoinGame;
class PacketLoginStart;
class PacketLoginSuccess;
class PacketPing;
class PacketPluginMessage;
class PacketRequest;
class PlayerConnection;

class PacketHandler {
    friend class PlayerConnection;

public:
    PacketHandler(PlayerConnection*);

    ~PacketHandler();

    void handleHandshake(PacketHandshake*);

    void handleRequest(PacketRequest*);

    void handlePing(PacketPing*);

    void handleLoginStart(PacketLoginStart*);

    void handleEncryptionResponse(PacketEncryptionResponse*);

    void handleLoginSuccess(PacketLoginSuccess*);

    void handlePluginMessage(PacketPluginMessage*);

    void handleJoinGame(PacketJoinGame*);

private:
    PlayerConnection *connect;
    string_t serverId;
    ubytes_t verifyToken;
    Profile *profile;
};

#endif /* defined(__Proxy__PacketHandler__) */
