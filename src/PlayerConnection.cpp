#include "PlayerConnection.h"

#include "ChatMessage.h"
#include "Compression.h"
#include "Logger.h"
#include "PacketDisconnect.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketLoginStart.h"
#include "PacketLoginSuccess.h"
#include "PacketPing.h"
#include "PacketPlayerListItem.h"
#include "PacketSpawnPlayer.h"
#include "PacketRequest.h"

#include <typeinfo>

PlayerConnection::PlayerConnection(ClientSocket *socket) : cSocket(socket), sSocket(nullptr),
        closed(false), phase(HANDSHAKE), encryption(false), compression(false) {
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
    if (!closed) {
        closed = true;
        try {
            cSocket->close();
        } catch (const Socket::SocketCloseException &e) {}
        if (sSocket != nullptr)
            try {
                sSocket->close();
            } catch (const Socket::SocketCloseException &e) {}
    }
}

bool PlayerConnection::isClosed() {
    return closed;
}

void PlayerConnection::runClient() {
    try {
        ubyte_t buffer[BUFFER_SIZE];
        while (!closed) {
            size_t rcvd = cSocket->receive(buffer, sizeof(buffer));
            cReadBuffer.setPosition(cReadBuffer.getLimit());
            if (encryption) {
                ubyte_t decrypt[rcvd];
                aes_crypt_cfb8(&aes_dec, AES_DECRYPT, rcvd, iv_dec, buffer, decrypt);
                cReadBuffer.put(decrypt, rcvd);
            } else
                cReadBuffer.put(buffer, rcvd);
            cReadBuffer.rewind();
            try {
                while (cReadBuffer.getPosition() < cReadBuffer.getLimit()) {
                    varint_t packetLength;
                    cReadBuffer.getVarInt(packetLength);
                    if (cReadBuffer.getLimit() - cReadBuffer.getPosition() >= packetLength) {
                        varint_t start = cReadBuffer.getPosition();
                        if (compression) {
                            varint_t dataLength;
                            cReadBuffer.getVarInt(dataLength);
                            packetLength -= getSize(dataLength);
                            start = cReadBuffer.getPosition();
                            if (dataLength > 0) {
                                auto inflated = Compression::inflateZlib(cReadBuffer.getData() + cReadBuffer.getPosition(), dataLength);
                                cReadBuffer.shift(inflated.second - packetLength);
                                cReadBuffer.put(inflated.first, inflated.second);
                                cReadBuffer.setPosition(start);
                                packetLength = inflated.second;
                                delete inflated.first;
                            }
                        }
                        varint_t packetId;
                        cReadBuffer.getVarInt(packetId);
                        Packet *packet = nullptr;
                        switch (phase) {
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
                        if (phase == PLAY) {
                            sendToServer(cReadBuffer.getData() + start, packetLength);
                            cReadBuffer.setPosition(start + packetLength);
                            cReadBuffer.compact();
                            continue;
                        }
                        if (packet == nullptr) {
                            disconnect("ID de paquet invalide : " + std::to_string(packetId));
                            break;
                        } else {
                            packet->setPacketLength(packetLength);
                            packet->read(cReadBuffer);
                            Logger::info() << "<Client -> Proxy> " << typeid(*packet).name() << std::endl;
                            packet->handle(handler);
                            delete packet;
                            cReadBuffer.compact();
                        }
                    } else
                        break;
                }
            } catch (const ByteBuffer::BufferUnderflowException &e) {}
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger::info() << "<Client -> Proxy> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::runServer() {
    try {
        ubyte_t buffer[BUFFER_SIZE];
        while (!closed) {
            size_t rcvd = sSocket->receive(buffer, sizeof(buffer));
            sReadBuffer.setPosition(sReadBuffer.getLimit());
            sReadBuffer.put(buffer, rcvd);
            sReadBuffer.rewind();
            try {
                while (sReadBuffer.getPosition() < sReadBuffer.getLimit()) {
                    varint_t packetLength;
                    sReadBuffer.getVarInt(packetLength);
                    size_t start = sReadBuffer.getPosition();
                    if (sReadBuffer.getLimit() - sReadBuffer.getPosition() >= packetLength) {
                        varint_t packetId;
                        sReadBuffer.getVarInt(packetId);
                        Packet *packet = nullptr;
                        switch (phase) {
                            case LOGIN:
                                if (packetId == 0x02)
                                    packet = new PacketLoginSuccess();
                                break;
                            case PLAY:
                                if (packetId == 0x0c)
                                    packet = new PacketSpawnPlayer();
                                else if (packetId == 0x38)
                                    packet = new PacketPlayerListItem();
                                break;
                            default:
                                break;
                        }
                        if (packet == nullptr) {
                            sendToClient(sReadBuffer.getData() + start, packetLength);
                            sReadBuffer.setPosition(start + packetLength);
                            sReadBuffer.compact();
                            continue;
                        }
                        packet->setPacketLength(packetLength);
                        packet->read(sReadBuffer);
                        Logger::info() << "<Proxy <- Serveur> " << typeid(*packet).name() << std::endl;
                        packet->handle(handler);
                        delete packet;
                        sReadBuffer.compact();
                    } else
                        break;
                }
            } catch (const ByteBuffer::BufferUnderflowException &e) {}
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger::info() << "<Proxy <- Server> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::sendToClient(Packet *packet) {
    try {
        if (!closed) {
            cWriteBuffer.clear();
            cWriteBuffer.putVarInt(packet->getPacketId());
            packet->write(cWriteBuffer);
            Logger::info() << "<Client <- Proxy> " << typeid(*packet).name() << std::endl;
            varint_t packetLength = cWriteBuffer.getLimit();
            if (compression) {
                varint_t dataLength = 0;
                if (packetLength >= 256) {
                    dataLength = packetLength;
                    auto deflated = Compression::deflateZLib(cWriteBuffer.getData(), packetLength);
                    cWriteBuffer.clear();
                    cWriteBuffer.put(deflated.first, deflated.second);
                    delete deflated.first;
                    packetLength = cWriteBuffer.getLimit();
                }
                packetLength += getSize(dataLength);
                cWriteBuffer.rewind();
                cWriteBuffer.shift(getSize(packetLength) + getSize(dataLength));
                cWriteBuffer.rewind();
                cWriteBuffer.putVarInt(packetLength);
                cWriteBuffer.putVarInt(dataLength);
                cWriteBuffer.rewind();
            } else {
                cWriteBuffer.rewind();
                cWriteBuffer.shift(getSize(packetLength));
                cWriteBuffer.rewind();
                cWriteBuffer.putVarInt(packetLength);
                cWriteBuffer.rewind();
            }
            if (encryption) {
                ubyte_t encrypt[cWriteBuffer.getLimit()];
                aes_crypt_cfb8(&aes_enc, AES_ENCRYPT, cWriteBuffer.getLimit(), iv_enc, cWriteBuffer.getData(), encrypt);
                cWriteBuffer.clear();
                cWriteBuffer.put(encrypt, sizeof(encrypt));
                cWriteBuffer.rewind();
            }
            cSocket->transmit(cWriteBuffer.getData(), cWriteBuffer.getLimit());
            delete packet;
        }
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "<Client <- Proxy> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::sendToClient(ubyte_t *packetData, varint_t packetLength) {
    try {
        if (!closed) {
            cWriteBuffer.clear();
            cWriteBuffer.put(packetData, packetLength);
            if (compression) {
                varint_t dataLength = 0;
                if (packetLength >= 256) {
                    dataLength = packetLength;
                    auto deflated = Compression::deflateZLib(cWriteBuffer.getData(), packetLength);
                    cWriteBuffer.clear();
                    cWriteBuffer.put(deflated.first, deflated.second);
                    delete deflated.first;
                    packetLength = cWriteBuffer.getLimit();
                }
                packetLength += getSize(dataLength);
                cWriteBuffer.rewind();
                cWriteBuffer.shift(getSize(packetLength) + getSize(dataLength));
                cWriteBuffer.rewind();
                cWriteBuffer.putVarInt(packetLength);
                cWriteBuffer.putVarInt(dataLength);
                cWriteBuffer.rewind();
            } else {
                cWriteBuffer.rewind();
                cWriteBuffer.shift(getSize(packetLength));
                cWriteBuffer.rewind();
                cWriteBuffer.putVarInt(packetLength);
                cWriteBuffer.rewind();
            }
            if (encryption) {
                ubyte_t encrypt[cWriteBuffer.getLimit()];
                aes_crypt_cfb8(&aes_enc, AES_ENCRYPT, cWriteBuffer.getLimit(), iv_enc, cWriteBuffer.getData(), encrypt);
                cWriteBuffer.clear();
                cWriteBuffer.put(encrypt, sizeof(encrypt));
                cWriteBuffer.rewind();
            }
            cSocket->transmit(cWriteBuffer.getData(), cWriteBuffer.getLimit());
        }
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "<Client <- Serveur> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::sendToServer(Packet *packet) {
    try {
        if (!closed) {
            sWriteBuffer.clear();
            sWriteBuffer.putVarInt(packet->getPacketId());
            packet->write(sWriteBuffer);
            Logger::info() << "<Proxy -> Serveur> " << typeid(*packet).name() << std::endl;
            varint_t packetLength = sWriteBuffer.getLimit();
            sWriteBuffer.rewind();
            sWriteBuffer.shift(getSize(packetLength));
            sWriteBuffer.rewind();
            sWriteBuffer.putVarInt(packetLength);
            sWriteBuffer.rewind();
            sSocket->transmit(sWriteBuffer.getData(), sWriteBuffer.getLimit());
            delete packet;
        }
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "<Proxy -> Serveur> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::sendToServer(ubyte_t *packetData, varint_t packetLength) {
    try {
        if (!closed) {
            sWriteBuffer.clear();
            sWriteBuffer.putVarInt(packetLength);
            sWriteBuffer.put(packetData, packetLength);
            sSocket->transmit(sWriteBuffer.getData(), sWriteBuffer.getLimit());
        }
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "<Client -> Serveur> déconnexion" << std::endl;
        close();
    }
}

void PlayerConnection::connect() {
    string_t ip = "127.0.0.1";
    ushort port = 25566;
    try {
        sSocket = new ClientSocket(Socket::SocketAddress(ip, port));
        sSocket->open();
        sThread = std::thread(&PlayerConnection::runServer, this);
        Logger::info() << "<Proxy -> Serveur> connexion" << std::endl;
    } catch (const ClientSocket::SocketConnectException &e) {
        Logger::warning() << "IMPOSSIBLE DE SE CONNECTER AU SERVEUR !" << std::endl;
        Logger::warning() << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger::warning() << "Peut-être que le serveur est hors-ligne ?" << std::endl;
        disconnect("Impossible de se connecter au serveur");
    }
}

void PlayerConnection::disconnect(string_t message) {
    Logger::info() << "<Client <- Proxy> déconnexion : '" << message << "'" << std::endl;
    PacketDisconnect *packet = new PacketDisconnect();
    packet->reason = (Chat() << message).getJSON();
    sendToClient(packet);
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
