#include "PacketHandler.h"

#include "PacketHandshake.h"
#include "PlayerConnection.h"

PacketHandler::PacketHandler(PlayerConnection *connect) : connect(connect) {}

void PacketHandler::handleHandshake(PacketHandshake *packet) {
    switch (packet->nextState) {
        case PacketFactory::STATUS:
            connect->phase = PacketFactory::STATUS;
        case PacketFactory::LOGIN:
            break;
        default:
            break;
    }
}

void PacketHandler::handleRequest(PacketRequest*) {}
