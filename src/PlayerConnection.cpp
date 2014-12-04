#include "PlayerConnection.h"

#include "ChatMessage.h"
#include "Compression.h"
#include "Logger.h"
#include "PacketDisconnect.h"
#include "PacketEncryptionResponse.h"
#include "PacketHandshake.h"
#include "PacketLoginStart.h"
#include "PacketPing.h"
#include "PacketRequest.h"

#include <typeinfo>

PlayerConnection::PlayerConnection(ClientSocket *socket) : socket(socket), closed(false), phase(HANDSHAKE),
        encryption(false), compression(false) {
    thread = std::thread(&PlayerConnection::run, this);
    handler = new PacketHandler(this);
}

PlayerConnection::~PlayerConnection() {
    delete handler;
    delete socket;
}

void PlayerConnection::join() {
    thread.join();
}

void PlayerConnection::close() {
    if (!closed) {
        closed = true;
        try {
            socket->close();
        } catch (const Socket::SocketCloseException &e) {}
    }
}

bool PlayerConnection::isClosed() {
    return closed;
}

void PlayerConnection::run() {
    try {
        ubyte_t buffer[BUFFER_SIZE];
        while (!closed) {
            size_t rcvd = socket->receive(buffer, sizeof(buffer));
            readBuffer.setPosition(readBuffer.getLimit());
            if (encryption) {
                ubyte_t decrypt[rcvd];
                aes_crypt_cfb8(&aes_dec, AES_DECRYPT, rcvd, iv_dec, buffer, decrypt);
                readBuffer.put(decrypt, rcvd);
            } else
                readBuffer.put(buffer, rcvd);
            readBuffer.rewind();
            try {
                while (readBuffer.getPosition() < readBuffer.getLimit()) {
                    varint_t packetLength;
                    readBuffer.getVarInt(packetLength);
                    if (readBuffer.getLimit() - readBuffer.getPosition() >= packetLength) {
                        if (compression) {
                            varint_t dataLength;
                            readBuffer.getVarInt(dataLength);
                            if (dataLength == 0)
                                packetLength -= getSize(dataLength);
                            else {
                                auto inflated = Compression::inflateZlib(readBuffer.getData() + readBuffer.getPosition(), dataLength);
                                readBuffer.clear();
                                readBuffer.put(inflated.first, inflated.second);
                                delete inflated.first;
                                packetLength = dataLength;
                            }
                        }
                        varint_t packetId;
                        readBuffer.getVarInt(packetId);
                        ClientPacket *packet = nullptr;
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
                        if (packet == nullptr) {
                            disconnect("ID de paquet invalide : " + std::to_string(packetId));
                            break;
                        } else {
                            packet->read(readBuffer);
                            Logger::info() << "/" << socket->getIP() << ":" << socket->getPort()
                                << " a envoyé un paquet " << typeid(*packet).name() << std::endl;
                            packet->handle(handler);
                            delete packet;
                            readBuffer.compact();
                        }
                    } else
                        break;
                }
            } catch (const ByteBuffer::BufferUnderflowException &e) {}
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger::info() << "/" << socket->getIP() << ":" << socket->getPort() << " s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::sendPacket(ServerPacket *packet) {
    try {
        if (!closed) {
            writeBuffer.clear();
            writeBuffer.putVarInt(packet->getPacketId());
            packet->write(writeBuffer);
            Logger::info() << "/" << socket->getIP() << ":" << socket->getPort()
                << " a reçu un paquet " << typeid(*packet).name() << std::endl;
            varint_t length = writeBuffer.getLimit();
            if (compression) {
                varint_t dataLength;
                varint_t packetLength;
                if (length < 256) {
                    dataLength = 0;
                    packetLength = length;
                } else {
                    auto deflated = Compression::deflateZLib(writeBuffer.getData(), length);
                    writeBuffer.clear();
                    writeBuffer.put(deflated.first, deflated.second);
                    delete deflated.first;
                    dataLength = length;
                    packetLength = writeBuffer.getLimit();
                }
                packetLength += getSize(dataLength);
                writeBuffer.shift(getSize(packetLength) + getSize(dataLength));
                writeBuffer.rewind();
                writeBuffer.putVarInt(packetLength);
                writeBuffer.putVarInt(dataLength);
                writeBuffer.rewind();
            } else {
                writeBuffer.shift(getSize(length));
                writeBuffer.rewind();
                writeBuffer.putVarInt(length);
                writeBuffer.rewind();
            }
            if (encryption) {
                ubyte_t encrypt[writeBuffer.getLimit()];
                aes_crypt_cfb8(&aes_enc, AES_ENCRYPT, writeBuffer.getLimit(), iv_enc, writeBuffer.getData(), encrypt);
                writeBuffer.clear();
                writeBuffer.put(encrypt, sizeof(encrypt));
                writeBuffer.rewind();
            }
            socket->transmit(writeBuffer.getData(), writeBuffer.getLimit());
            delete packet;
        }
    } catch (const ClientSocket::SocketWriteException &e) {
        std::cout<< e.what() << std::endl;
        Logger::info() << "/" << socket->getIP() << ":" << socket->getPort() << " s'est déconnecté" << std::endl;
        close();
    }
}

void PlayerConnection::disconnect(string_t message) {
    Logger::info() << "/" << socket->getIP() << ":" << socket->getPort()
        << " a été déconnecté : '" << message << "'" << std::endl;
    PacketDisconnect *packet = new PacketDisconnect();
    packet->reason = (Chat() << message).getJSON();
    sendPacket(packet);
    close();
}

void PlayerConnection::setup(ubytes_t *iv) {
    aes_init(&aes_enc);
    memcpy(this->iv_enc, iv->data(), 16);
    aes_setkey_enc(&aes_enc, this->iv_enc, 128);
    aes_init(&aes_dec);
    memcpy(this->iv_dec, iv->data(), 16);
    aes_setkey_enc(&aes_dec, this->iv_dec, 128);
}
