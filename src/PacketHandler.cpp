#include "PacketHandler.h"

#include "PacketHandshake.h"
#include "PacketPing.h"
#include "PacketResponse.h"
#include "PlayerConnection.h"

#include "json11/json11.hpp"

PacketHandler::PacketHandler(PlayerConnection *connect) : connect(connect) {}

void PacketHandler::handleHandshake(PacketHandshake *packet) {
    switch (packet->nextState) {
        case PlayerConnection::STATUS:
            connect->phase = PlayerConnection::STATUS;
            break;
        default:
            break;
    }
}

void PacketHandler::handleRequest(PacketRequest*) {
    PacketResponse *packet = new PacketResponse();
    json11::Json response = json11::Json::object {
        {"version", json11::Json::object {
            {"name", "1.8.1"},
            {"protocol", 47}
        }}, {"players", json11::Json::object {
            {"max", 20},
            {"online", 0}
        }}, {"description", json11::Json::object {
            {"text", "A Minecraft Server"}
        }}
    };
    packet->response = response.dump();
    connect->sendPacket(packet);
}

void PacketHandler::handlePing(PacketPing *packet) {
    PacketPing *pingPacket = new PacketPing();
    pingPacket->time = packet->time;
    connect->sendPacket(pingPacket);
    connect->close();
}
