#include "PacketHandler.h"

#include "Encryption.h"
#include "Logger.h"
#include "PacketEncryptionRequest.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketJoinGame.h"
#include "PacketLoginStart.h"
#include "PacketLoginSuccess.h"
#include "PacketPing.h"
#include "PacketPluginMessage.h"
#include "PacketRespawn.h"
#include "PacketResponse.h"
#include "PacketSetCompression.h"
#include "PlayerConnection.h"
#include "Proxy.h"

#include "mongo/bson/bson.h"
#include "polarssl/md5.h"

#include <iomanip>
#include <sstream>

PacketHandler::PacketHandler(PlayerConnection *connect) : connect(connect), profile(nullptr) {}

PacketHandler::~PacketHandler() {
    if (profile != nullptr)
        delete profile;
}

void PacketHandler::handleHandshake(PacketHandshake *packet) {
    switch (packet->nextState) {
        case PlayerConnection::STATUS:
            connect->cPhase = PlayerConnection::STATUS;
            break;
        case PlayerConnection::LOGIN:
            if (packet->protocolVersion < 47)
                connect->disconnect("Client trop ancien, merci d'utiliser au moins la 1.8");
            else if (packet->protocolVersion > 47)
                connect->disconnect("Serveur trop ancien, merci d'utiliser au plus la 1.8.1");
            else
                connect->cPhase = PlayerConnection::LOGIN;
            break;
        default:
            connect->disconnect("État invalide : " + std::to_string(packet->nextState));
            break;
    }
}

void PacketHandler::handleRequest(PacketRequest*) {
    PacketResponse *packet = new PacketResponse();
    mongo::BSONObj response = BSON(
        "version" << BSON("name" << "1.8.1" << "protocol" << 47)
        << "players" << BSON("max" << 20 << "online" << 0)
        << "description" << BSON("text" << "A Minecraft Server"));
    packet->response = response.jsonString();
    connect->sendClient(packet);
}

void PacketHandler::handlePing(PacketPing *packet) {
    PacketPing *pingPacket = new PacketPing();
    pingPacket->time = packet->time;
    connect->sendClient(pingPacket);
    connect->close();
}

void PacketHandler::handleLoginStart(PacketLoginStart *packet) {
    profile = new Profile(packet->name);
    PacketEncryptionRequest *encryptPacket = new PacketEncryptionRequest();
    encryptPacket->serverId = this->serverId;
    encryptPacket->publicKey = Encryption::getPublicKey();
    int x = (int) (intptr_t) this;
    verifyToken = {(ubyte_t) (x & 0xff), (ubyte_t) ((x >> 8) & 0xff),
        (ubyte_t) ((x >> 16) & 0xff), (ubyte_t) ((x >> 24) & 0xff)};
    encryptPacket->verifyToken = verifyToken;
    connect->sendClient(encryptPacket);
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
            if (Mojang::authentificate(profile, serverId, sharedSecret, Encryption::getPublicKey())) {
                connect->encryption = true;
                Proxy::getDatabase()->addProfile(profile);
                Logger() << "L'UUID du joueur " << profile->getName() << " est " << profile->getUUID() << std::endl;
                PacketSetCompression *compressPacket = new PacketSetCompression();
                compressPacket->threshold = COMPRESSION_THRESHOLD;
                connect->sendClient(compressPacket);
                connect->compression = true;
                //TODO Passer par le Mongo pour connaître le serveur par défaut
                //connect->connect(Proxy::getConfig()->getServers()[Proxy::getConfig()->getDefaultServer()]);
                connect->connect({"0.0.0.0", 25566});
            } else {
                connect->disconnect("Impossible de vérifier le nom d'utilisateur !");
                Logger(LogLevel::WARNING) << "'" << profile->getName() << "' a essayé de rejoindre avec une session invalide" << std::endl;
            }
        } catch (const Mojang::SSLException &e) {
            connect->disconnect("Les serveurs d'authentification sont hors-ligne, merci de réessayer plus tard");
            Logger(LogLevel::SEVERE) << "Impossible de vérifier le nom d'utilisateur car les serveurs sont hors-ligne" << std::endl;
        }
    } else
        connect->disconnect("Nonce invalide !");
}

void PacketHandler::handleLoginSuccess(PacketLoginSuccess*) {
    if (connect->cPhase != PlayerConnection::PLAY) {
        PacketLoginSuccess *successPacket = new PacketLoginSuccess();
        successPacket->username = profile->getName();
        successPacket->uuid = profile->getUUID();
        connect->sendClient(successPacket);
    }
    connect->sPhase = PlayerConnection::PLAY;
}

void PacketHandler::handlePluginMessage(PacketPluginMessage *packet) {
    //TODO Passer par le Mongo pour connaître les serveurs
    /*if (packet->channel == "MF|GetServers") {
        PacketPluginMessage *pluginPacket = new PacketPluginMessage();
        pluginPacket->channel = packet->channel;
        PacketBuffer buffer;
        buffer.putVarInt(Proxy::getConfig()->getServers().size());
        for (auto server : Proxy::getConfig()->getServers())
            buffer.putString(server.first);
        pluginPacket->data = ubytes_t(buffer.getArray(), buffer.getArray() + buffer.getSize());
        connect->sendServer(pluginPacket);
    } else if (packet->channel == "MF|Connect") {
        PacketBuffer buffer;
        buffer.putUBytes(packet->data);
        buffer.setPosition(0);
        string_t server;
        buffer.getString(server);
        connect->connect(Proxy::getConfig()->getServers()[server]);
    } else {
        PacketPluginMessage *pluginPacket = new PacketPluginMessage();
        pluginPacket->channel = packet->channel;
        pluginPacket->data = packet->data;
        connect->sendClient(pluginPacket);
    }*/
}

void PacketHandler::handleJoinGame(PacketJoinGame *packet) {
    if (connect->cPhase == PlayerConnection::PLAY) {
        PacketRespawn *respawnPacket = new PacketRespawn();
        respawnPacket->dimension = packet->dimension;
        respawnPacket->difficulty = packet->difficulty;
        respawnPacket->gamemode = packet->gamemode;
        respawnPacket->levelType = packet->levelType;
        connect->sendClient(respawnPacket);
    } else {
        connect->cPhase = PlayerConnection::PLAY;
        PacketJoinGame *joinPacket = new PacketJoinGame();
        joinPacket->entityId = packet->entityId;
        joinPacket->gamemode = packet->gamemode;
        joinPacket->dimension = packet->dimension;
        joinPacket->maxPlayers = packet->maxPlayers;
        joinPacket->levelType = packet->levelType;
        joinPacket->reducedDebugInfo = packet->reducedDebugInfo;
        connect->sendClient(joinPacket);
    }
}
