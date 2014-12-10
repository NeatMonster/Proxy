#ifndef __Proxy__PacketHandler__
#define __Proxy__PacketHandler__

#include "Mojang.h"
#include "Types.h"

class PacketEncryptionResponse;
class PacketHandshake;
class PacketLoginStart;
class PacketLoginSuccess;
class PacketPing;
class PacketPlayerListItem;
class PacketRequest;
class PacketSpawnPlayer;
class PlayerConnection;

class PacketHandler {
public:
    PacketHandler(PlayerConnection*);

    void handleHandshake(PacketHandshake*);

    void handleRequest(PacketRequest*);

    void handlePing(PacketPing*);

    void handleLoginStart(PacketLoginStart*);

    void handleEncryptionResponse(PacketEncryptionResponse*);

    void handleLoginSuccess(PacketLoginSuccess*);

    void handlePlayerListItem(PacketPlayerListItem*);

    void handleSpawnPlayer(PacketSpawnPlayer*);

private:
    PlayerConnection *connect;
    string_t username;
    string_t serverId;
    ubytes_t verifyToken;
    Profile profile;
};

#endif /* defined(__Proxy__PacketHandler__) */
