#include "PlayerConnection.h"

#include "ChatMessage.h"
#include "Logger.h"
#include "PacketDisconnect.h"
#include "PacketHandshake.h"
#include "PacketPing.h"
#include "PacketRequest.h"

#include <typeinfo>

PlayerConnection::PlayerConnection(ClientSocket *socket) : socket(socket), closed(false), phase(HANDSHAKE) {
    thread = std::thread(&PlayerConnection::run, this);
    handler = new PacketHandler(this);
}

PlayerConnection::~PlayerConnection() {
    delete handler;
    delete socket;
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
            readBuffer.put(buffer, rcvd);
            readBuffer.rewind();
            try {
                while (readBuffer.getPosition() < readBuffer.getLimit()) {
                    varint_t packetLength;
                    readBuffer.getVarInt(packetLength);
                    if (readBuffer.getLimit() - readBuffer.getPosition() >= packetLength) {
                        varint_t packetId;
                        readBuffer.getVarInt(packetId);
                        ClientPacket *packet = nullptr;
                        switch (phase.load()) {
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
            varint_t packetLength = writeBuffer.getLimit();
            writeBuffer.shift(getSize(packetLength));
            writeBuffer.rewind();
            writeBuffer.putVarInt(packetLength);
            writeBuffer.rewind();
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
        << " a été déconnecté : " << message << std::endl;
    PacketDisconnect *packet = new PacketDisconnect();
    packet->reason = (Chat() << message).getJSON();
    sendPacket(packet);
    close();
}
