#include "PlayerConnection.h"

#include "Logger.h"

#include <typeinfo>

PlayerConnection::PlayerConnection(ClientSocket *socket) : socket(socket), closed(false) {
    readThread = std::thread(&PlayerConnection::runRead, this);
    writeThread = std::thread(&PlayerConnection::writeThread, this);
}

PlayerConnection::~PlayerConnection() {
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

void PlayerConnection::runRead() {
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
                        if (factories[phase].hasPacket(packetId)) {
                            ClientPacket *readPacket = factories[phase].createPacket(packetId);
                            readPacket->read(readBuffer);
                            // Pour l'instant, on se contente d'afficher le nom du paquet.
                            Logger::info() << "/" << socket->getIP() << ":" << socket->getPort()
                                << " a envoyé un paquet " << typeid(*readPacket).name() << std::endl;
                            delete readPacket;
                            readBuffer.compact();
                        } else {
                            Logger::warning() << packetId << " n'est pas ID de paquet valide" << std::endl;
                            break;
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

void PlayerConnection::runWrite() {
    try {
        // Rien à faire pour le moment.
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "/" << socket->getIP() << ":" << socket->getPort() << " s'est déconnecté" << std::endl;
        close();
    }
}

PacketFactory PlayerConnection::factories[4] = {
    PacketFactory(PacketFactory::HANDSHAKE),
    PacketFactory(PacketFactory::STATUS),
    PacketFactory(PacketFactory::LOGIN),
    PacketFactory(PacketFactory::PLAY)
};
