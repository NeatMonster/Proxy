#include "PacketHandler.h"

#include "Encryption.h"
#include "Logger.h"
#include "PacketEncryptionRequest.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketLoginStart.h"
#include "PacketLoginSuccess.h"
#include "PacketPing.h"
#include "PacketPlayerListItem.h"
#include "PacketPluginMessage.h"
#include "PacketResponse.h"
#include "PacketSetCompression.h"
#include "PacketSpawnPlayer.h"
#include "PlayerConnection.h"
#include "Proxy.h"

#include "json11/json11.hpp"
#include "polarssl/md5.h"

#include <iomanip>
#include <sstream>

PacketHandler::PacketHandler(PlayerConnection *connect) : connect(connect) {}

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
            break;
        default:
            connect->disconnect("État invalide : " + std::to_string(packet->nextState));
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
    connect->sendToClient(packet);
}

void PacketHandler::handlePing(PacketPing *packet) {
    PacketPing *pingPacket = new PacketPing();
    pingPacket->time = packet->time;
    connect->sendToClient(pingPacket);
    connect->close();
}

void PacketHandler::handleLoginStart(PacketLoginStart *packet) {
    username = packet->name;
    PacketEncryptionRequest *encryptPacket = new PacketEncryptionRequest();
    encryptPacket->serverId = this->serverId;
    encryptPacket->publicKey = Encryption::getPublicKey();
    int x = (int) (intptr_t) this;
    verifyToken = {(ubyte_t) (x & 0xff), (ubyte_t) ((x >> 8) & 0xff),
        (ubyte_t) ((x >> 16) & 0xff), (ubyte_t) ((x >> 24) & 0xff)};
    encryptPacket->verifyToken = verifyToken;
    connect->sendToClient(encryptPacket);
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
            if (Mojang::authentificate(&profile, username, serverId, sharedSecret, Encryption::getPublicKey())) {
                connect->encryption = true;
                string_t s = "OfflinePlayer:" + profile.name;
                md5_context ctx;
                md5_init(&ctx);
                md5_starts(&ctx);
                md5_update(&ctx, (ubyte_t*) s.data(), s.size());
                ubytes_t hash(16);
                md5_finish(&ctx, hash.data());
                md5_free(&ctx);
                hash[6] &= 0x0f;
                hash[6] |= 0x30;
                hash[8] &= 0x3f;
                hash[8] |= 0x80;
                std::stringstream ss;
                for (int i = 0; i < 16; i++) {
                    if (i == 4 || i == 6 || i == 8 || i == 10)
                        ss << "-";
                    ss << std::setw(2) << std::hex << std::setfill('0') << (int) hash[i];
                }
                Proxy::addProfile(ss.str(), profile);
                Logger() << "L'UUID du joueur " << profile.name << " est " << profile.uuid << std::endl;
                PacketSetCompression *compressPacket = new PacketSetCompression();
                compressPacket->threshold = COMPRESSION_THRESHOLD;
                connect->sendToClient(compressPacket);
                connect->compression = true;
                connect->connect();
                PacketHandshake *handPacket = new PacketHandshake();
                handPacket->protocolVersion = 47;
                handPacket->serverAddress = "localhost";
                handPacket->serverPort = 25566;
                handPacket->nextState = PlayerConnection::LOGIN;
                connect->sendToServer(handPacket);
                PacketLoginStart *loginPacket = new PacketLoginStart();
                loginPacket->name = username;
                connect->sendToServer(loginPacket);
            } else {
                connect->disconnect("Impossible de vérifier le nom d'utilisateur !");
                Logger(LogLevel::WARNING) << "'" << username << "' a essayé de rejoindre avec une session invalide" << std::endl;
            }
        } catch (const Mojang::SSLException &e) {
            connect->disconnect("Les serveurs d'authentification sont hors-ligne, merci de réessayer plus tard");
            Logger(LogLevel::SEVERE) << "Impossible de vérifier le nom d'utilisateur car les serveurs sont hors-ligne" << std::endl;
        }
    } else
        connect->disconnect("Nonce invalide !");
}

void PacketHandler::handleLoginSuccess(PacketLoginSuccess*) {
    PacketLoginSuccess *successPacket = new PacketLoginSuccess();
    successPacket->username = profile.name;
    successPacket->uuid = profile.uuid;
    connect->sendToClient(successPacket);
    connect->phase = PlayerConnection::PLAY;
}

void PacketHandler::handlePlayerListItem(PacketPlayerListItem *packet) {
    PacketPlayerListItem *listPacket = new PacketPlayerListItem();
    listPacket->type = packet->type;
    for (PacketPlayerListItem::Action &action : packet->actions) {
        PacketPlayerListItem::Action newAction;
        newAction.profile = Proxy::getProfile(action.profile.uuid);
        newAction.gameMode = action.gameMode;
        newAction.ping = action.ping;
        newAction.hasDisplayName = action.hasDisplayName;
        newAction.displayName = action.displayName;
        listPacket->actions.push_back(newAction);
    }
    connect->sendToClient(listPacket);
}

void PacketHandler::handleSpawnPlayer(PacketSpawnPlayer *packet) {
    PacketSpawnPlayer *spawnPacket = new PacketSpawnPlayer();
    spawnPacket->entityId = packet->entityId;
    spawnPacket->uuid = Proxy::getProfile(packet->uuid).uuid;
    spawnPacket->x = packet->x;
    spawnPacket->y = packet->y;
    spawnPacket->z = packet->z;
    spawnPacket->yaw = packet->yaw;
    spawnPacket->pitch = packet->pitch;
    spawnPacket->currentItem = packet->currentItem;
    spawnPacket->metadata = packet->metadata;
    connect->sendToClient(spawnPacket);
}

void PacketHandler::handlePluginMessage(PacketPluginMessage *packet) {
    if (packet->channel == "MF|GetServers") {
        PacketPluginMessage *pluginPacket = new PacketPluginMessage();
        pluginPacket->channel = packet->channel;
        PacketBuffer buffer;
        buffer.putVarInt(Proxy::getConfig()->getServers().size());
        for (auto server : Proxy::getConfig()->getServers())
            buffer.putString(server.first);
        pluginPacket->data = ubytes_t(buffer.getArray(), buffer.getArray() + buffer.getSize());
        connect->sendToServer(pluginPacket);
    } else {
        PacketPluginMessage *pluginPacket = new PacketPluginMessage();
        pluginPacket->channel = packet->channel;
        pluginPacket->data = packet->data;
        connect->sendToClient(pluginPacket);
    }
}
