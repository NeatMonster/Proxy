#include "PacketHandler.h"

#include "Encryption.h"
#include "Logger.h"
#include "PacketEncryptionRequest.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketLoginStart.h"
#include "PacketPing.h"
#include "PacketResponse.h"
#include "PlayerConnection.h"

#include "json11/json11.hpp"

PacketHandler::PacketHandler(PlayerConnection *connect) : connect(connect) {}

PacketHandler::~PacketHandler() {
    if (profile != nullptr)
        delete profile;
}

void PacketHandler::handleHandshake(PacketHandshake *packet) {
    switch (packet->nextState) {
        case PlayerConnection::STATUS:
            connect->phase = PlayerConnection::STATUS;
            break;
        case PlayerConnection::LOGIN:
            if (packet->protocolVersion < 47)
                connect->disconnect("Client trop ancien, merci d'utiliser au moins la 1.8");
            else if (packet->protocolVersion > 47)
                connect->disconnect("Serveur trop ancien, merci d'utiliser au plus la 1.8.1");
            else
                connect->phase = PlayerConnection::LOGIN;
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

void PacketHandler::handleLoginStart(PacketLoginStart *packet) {
    username = packet->name;
    PacketEncryptionRequest *encryptPacket = new PacketEncryptionRequest();
    encryptPacket->serverId = this->serverId;
    encryptPacket->publicKey = Encryption::getPublicKey();
    int x = (int) (intptr_t) this;
    verifyToken = {(ubyte_t) (x & 0xff), (ubyte_t) ((x >> 8) & 0xff), (ubyte_t) ((x >> 16) & 0xff), (ubyte_t) ((x >> 24) & 0xff)};
    encryptPacket->verifyToken = verifyToken;
    connect->sendPacket(encryptPacket);
}

void PacketHandler::handleEncryptionResponse(PacketEncryptionResponse *packet) {
    size_t tokenLength = 4;
    ubytes_t verifyToken(tokenLength);
    Encryption::decrypt(packet->verifyToken.data(), verifyToken.data(), &tokenLength);
    if (this->verifyToken == verifyToken) {
        size_t secretLength = 16;
        ubytes_t sharedSecret(secretLength);
        Encryption::decrypt(packet->sharedSecret.data(), sharedSecret.data(), &secretLength);
        connect->setup(&sharedSecret);
        try {
            profile = Mojang::authentificate(username, serverId, sharedSecret, Encryption::getPublicKey());
            if (profile == nullptr) {
                connect->disconnect("Impossible de vérifier le nom d'utilisateur !");
                Logger::warning() << "'" << username << "' a essayé de rejoindre avec une session invalide" << std::endl;
                return;
            } else {
                connect->encryption = true;
                Logger::info() << "L'UUID du joueur " << profile->name << " est " << profile->uuid << std::endl;
            }
        } catch (const Mojang::SSLException &e) {
            connect->disconnect("Les serveurs d'authentification sont hors-ligne, merci de réessayer plus tard");
            Logger::severe() << "Impossible de vérifier le nom d'utilisateur car les serveurs sont hors-ligne" << std::endl;
            return;
        }
    } else
        connect->disconnect("Nonce invalide !");
}
