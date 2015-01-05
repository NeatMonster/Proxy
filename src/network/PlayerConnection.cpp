#include "PlayerConnection.h"

#include "ChatMessage.h"
#include "Compression.h"
#include "Logger.h"
#include "PacketDisconnect.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketJoinGame.h"
#include "PacketLoginStart.h"
#include "PacketLoginSuccess.h"
#include "PacketPing.h"
#include "PacketPlayerListItem.h"
#include "PacketPluginMessage.h"
#include "PacketSpawnPlayer.h"
#include "PacketRequest.h"
#include "Proxy.h"

#include <typeinfo>

PlayerConnection::PlayerConnection(ClientSocket *socket) : cSocket(socket), sSocket(nullptr), cClosed(false),
        sClosed(true), cPhase(HANDSHAKE), sPhase(HANDSHAKE), encryption(false), compression(false) {
    cThread = std::thread(&PlayerConnection::runClient, this);
    handler = new PacketHandler(this);
}

PlayerConnection::~PlayerConnection() {
    delete handler;
    delete cSocket;
    if (sSocket != nullptr)
        delete sSocket;
}

void PlayerConnection::join() {
    cThread.join();
    if (sThread.joinable())
        sThread.join();
}

void PlayerConnection::close() {
    if (!cClosed)
        try {
            cSocket->close();
            cClosed = true;
        } catch (const Socket::SocketCloseException &e) {}
    closeServer();
}

void PlayerConnection::closeServer() {
    if (!sClosed)
        try {
            sSocket->close();
            sClosed = true;
        } catch (const Socket::SocketCloseException &e) {}
}

bool PlayerConnection::isClosed() {
    return cClosed && sClosed;
}

string_t PlayerConnection::getName() {
    if (handler->username.empty())
        return "/" + cSocket->getIP() + ":" + std::to_string(cSocket->getPort());
    return handler->username;
}

void PlayerConnection::runClient() {
    try {
        size_t position = 0;
        while (!cClosed) {
            cReadBuffer.reserve(cReadBuffer.getLimit() + BUFFER_SIZE);
            size_t rcvd = cSocket->receive(cReadBuffer.getArray() + cReadBuffer.getLimit(), BUFFER_SIZE);
            cReadBuffer.setLimit(cReadBuffer.getLimit() + rcvd);
            cReadBuffer.setPosition(position);
            if (encryption)
                aes_crypt_cfb8(&aes_dec, AES_DECRYPT, rcvd, iv_dec, cReadBuffer.getArray() + position,
                               cReadBuffer.getArray() + position);
            try {
                while (cReadBuffer.getPosition() < cReadBuffer.getLimit()) {
                    varint_t packetLength;
                    cReadBuffer.getVarInt(packetLength);
                    if (cReadBuffer.getLimit() - cReadBuffer.getPosition() < packetLength)
                        break;
                    PacketBuffer *buffer = &cReadBuffer;
                    if (compression) {
                        varint_t dataLength;
                        cReadBuffer.getVarInt(dataLength);
                        packetLength -= getSize(dataLength);
                        if (dataLength) {
                            buffer = &inflateBuffer;
                            inflateBuffer.clear();
                            inflateBuffer.reserve(dataLength);
                            Compression::inflateZlib(cReadBuffer.getArray() + cReadBuffer.getPosition(), packetLength,
                                                     inflateBuffer.getArray(), dataLength);
                            inflateBuffer.setLimit(dataLength);
                            cReadBuffer.setPosition(cReadBuffer.getPosition() + packetLength);
                            packetLength = dataLength;
                        }
                    }
                    buffer->setMark(buffer->getPosition());
                    varint_t packetId;
                    buffer->getVarInt(packetId);
                    Packet *packet = nullptr;
                    switch (cPhase.load()) {
                        case HANDSHAKE:
                            if (packetId == 0x00)
                                packet = new PacketHandshake();
                            break;
                        case STATUS:
                            if (packetId == 0x00)
                                packet = new PacketRequest();
                            else if (packetId == 0x01)
                                packet = new PacketPing();
                            break;
                        case LOGIN:
                            if (packetId == 0x00)
                                packet = new PacketLoginStart();
                            else if (packetId == 0x01)
                                packet = new PacketEncryptionResponse();
                        default:
                            break;
                    }
                    if (cPhase == PLAY) {
                        if (sPhase == PLAY)
                            sendServer(buffer->getArray() + buffer->getMark(), packetLength);
                        buffer->setPosition(buffer->getMark() + packetLength);
                    } else if (packet == nullptr) {
                        disconnect("ID de paquet invalide : " + std::to_string(packetId));
                        break;
                    } else {
                        packet->setPacketLength(packetLength);
                        packet->read(*buffer);
                        Logger(LogLevel::DEBUG) << "<" << getName() << " -> Proxy> " << typeid(*packet).name() << std::endl;
                        packet->handle(handler);
                        delete packet;
                    }
                    if (cReadBuffer.getPosition() == cReadBuffer.getLimit())
                        cReadBuffer.clear();
                    position = cReadBuffer.getPosition();
                }
            } catch (const PacketBuffer::BufferUnderflowException &e) {}
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger() << "<" << getName() << " <-> Proxy> s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::runServer() {
    try {
        size_t position = 0;
        while (!sClosed) {
            sReadBuffer.reserve(sReadBuffer.getLimit() + BUFFER_SIZE);
            size_t rcvd = sSocket->receive(sReadBuffer.getArray() + sReadBuffer.getLimit(), BUFFER_SIZE);
            sReadBuffer.setLimit(sReadBuffer.getLimit() + rcvd);
            sReadBuffer.setPosition(position);
            try {
                while (sReadBuffer.getPosition() < sReadBuffer.getLimit()) {
                    varint_t packetLength;
                    sReadBuffer.getVarInt(packetLength);
                    if (sReadBuffer.getLimit() - sReadBuffer.getPosition() < packetLength)
                        break;
                    sReadBuffer.setMark(sReadBuffer.getPosition());
                    varint_t packetId;
                    sReadBuffer.getVarInt(packetId);
                    Packet *packet = nullptr;
                    switch (sPhase.load()) {
                        case LOGIN:
                            if (packetId == 0x02)
                                packet = new PacketLoginSuccess();
                            break;
                        case PLAY:
                            if (packetId == 0x01)
                                packet = new PacketJoinGame();
                            else if (packetId == 0x0c)
                                packet = new PacketSpawnPlayer();
                            else if (packetId == 0x38)
                                packet = new PacketPlayerListItem();
                            break;
                        default:
                            break;
                    }
                    if (packetId == 0x3f)
                        packet = new PacketPluginMessage();
                    if (packet == nullptr) {
                        sendClient(sReadBuffer.getArray() + sReadBuffer.getMark(), packetLength);
                        sReadBuffer.setPosition(sReadBuffer.getMark() + packetLength);
                    } else {
                        packet->setPacketLength(packetLength);
                        packet->read(sReadBuffer);
                        Logger(LogLevel::DEBUG) << "<Proxy <- Serveur> " << typeid(*packet).name() << std::endl;
                        packet->handle(handler);
                        delete packet;
                    }
                    if (sReadBuffer.getPosition() == sReadBuffer.getLimit())
                        sReadBuffer.clear();
                    position = sReadBuffer.getPosition();
                }
            } catch (const PacketBuffer::BufferUnderflowException &e) {}
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger() << "<Proxy <-> Serveur> s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::sendClient(Packet *packet) {
    if (cClosed)
        return;
    Logger(LogLevel::DEBUG) << "<" << getName() << " <- Proxy> " << typeid(*packet).name() << std::endl;
    cWriteBuffer.clear();
    cWriteBuffer.setPosition(6);
    varint_t packetId = packet->getServerPacketId();
    cWriteBuffer.putVarInt(packetId);
    packet->write(cWriteBuffer);
    delete packet;
    sendClient(cWriteBuffer.getLimit() - 6);
}

void PlayerConnection::sendClient(ubyte_t *packetData, varint_t packetLength) {
    if (cClosed)
        return;
    cWriteBuffer.clear();
    cWriteBuffer.setPosition(6);
    cWriteBuffer.put(packetData, packetLength);
    sendClient(packetLength);
}

void PlayerConnection::sendClient(varint_t packetLength) {
    try {
        cWriteBuffer.setPosition(6);
        PacketBuffer *buffer = &cWriteBuffer;
        if (compression) {
            varint_t dataLength = packetLength;
            if (dataLength >= COMPRESSION_THRESHOLD) {
                buffer = &deflateBuffer;
                deflateBuffer.clear();
                deflateBuffer.setPosition(5);
                deflateBuffer.putVarInt(dataLength);
                deflateBuffer.reserve(deflateBuffer.getPosition() + dataLength);
                size_t length = Compression::deflateZLib(cWriteBuffer.getArray() + 6, dataLength,
                                                         deflateBuffer.getArray() + deflateBuffer.getPosition(), dataLength);
                deflateBuffer.setLimit(deflateBuffer.getPosition() + length);
                deflateBuffer.setPosition(5);
                packetLength = deflateBuffer.getLimit() - 5;
            } else {
                packetLength++;
                cWriteBuffer.getArray()[5] = 0;
                cWriteBuffer.setPosition(5);
            }
        }
        buffer->setPosition(buffer->getPosition() - getSize(packetLength));
        buffer->setMark(buffer->getPosition());
        buffer->putVarInt(packetLength);
        if (encryption)
            aes_crypt_cfb8(&aes_dec, AES_ENCRYPT, buffer->getLimit() - buffer->getMark(), iv_enc,
                           buffer->getArray() + buffer->getMark(), buffer->getArray() + buffer->getMark());
        cSocket->transmit(buffer->getArray() + buffer->getMark(), buffer->getLimit() - buffer->getMark());
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger() << "<" << getName() << " <-> Proxy> s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::sendServer(Packet *packet) {
    try {
        if (sClosed)
            return;
        Logger(LogLevel::DEBUG) << "<Proxy -> Serveur> " << typeid(*packet).name() << std::endl;
        sWriteBuffer.clear();
        sWriteBuffer.setPosition(5);
        varint_t packetId = packet->getClientPacketId();
        sWriteBuffer.putVarInt(packetId);
        packet->write(sWriteBuffer);
        delete packet;
        varint_t packetLength = sWriteBuffer.getLimit() - 5;
        size_t position = 5 - getSize(packetLength);
        sWriteBuffer.setPosition(position);
        sWriteBuffer.putVarInt(packetLength);
        sSocket->transmit(sWriteBuffer.getArray() + position, sWriteBuffer.getLimit() - position);
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger() << "<Proxy <-> Serveur> s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::sendServer(ubyte_t *packetData, varint_t packetLength) {
    try {
        if (sClosed)
            return;
        sWriteBuffer.clear();
        sWriteBuffer.putVarInt(packetLength);
        sSocket->transmit(sWriteBuffer.getArray(), sWriteBuffer.getLimit());
        sSocket->transmit(packetData, packetLength);
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger() << "<Proxy <-> Serveur> s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::connect(std::pair<string_t, u_short> server) {
    try {
        if (!sClosed) {
            try {
                sSocket->close();
            } catch (const Socket::SocketCloseException &e) {}
            Logger() << "<Proxy <-> Serveur> s'est déconnecté" << std::endl;
            sReadBuffer.clear();
            sPhase = HANDSHAKE;
        }
        sSocket = new ClientSocket(Socket::SocketAddress(server.first, server.second));
        sSocket->open();
        if (sClosed) {
            sClosed = false;
            sThread = std::thread(&PlayerConnection::runServer, this);
        }
        PacketHandshake *handPacket = new PacketHandshake();
        handPacket->protocolVersion = 47;
        handPacket->serverAddress = server.first;
        handPacket->serverPort = server.second;
        handPacket->nextState = sPhase = LOGIN;
        sendServer(handPacket);
        PacketLoginStart *loginPacket = new PacketLoginStart();
        loginPacket->name = handler->username;
        sendServer(loginPacket);
        Logger() << "<Proxy <-> Serveur> s'est connecté" << std::endl;
    } catch (const ClientSocket::SocketConnectException &e) {
        Logger(LogLevel::WARNING) << "IMPOSSIBLE DE SE CONNECTER AU SERVEUR !" << std::endl;
        Logger(LogLevel::WARNING) << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger(LogLevel::WARNING) << "Peut-être que le serveur est hors-ligne ?" << std::endl;
        disconnect("Impossible de se connecter au serveur");
    }
}

void PlayerConnection::disconnect(string_t message) {
    PacketDisconnect *packet = new PacketDisconnect(cPhase);
    packet->reason = (Chat() << message).getJSON();
    sendClient(packet);
    packet = new PacketDisconnect(sPhase);
    packet->reason = (Chat() << message).getJSON();
    sendServer(packet);
    close();
}

void PlayerConnection::setup(ubytes_t *iv) {
    aes_init(&aes_enc);
    std::memcpy(this->iv_enc, iv->data(), 16);
    aes_setkey_enc(&aes_enc, this->iv_enc, 128);
    aes_init(&aes_dec);
    std::memcpy(this->iv_dec, iv->data(), 16);
    aes_setkey_enc(&aes_dec, this->iv_dec, 128);
}
