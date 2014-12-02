#include "PlayerConnection.h"

#include "Logger.h"

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
            ubyte_t b;
            while (readBuffer.getPosition() < readBuffer.getLimit()) {
                // On se contente d'afficher les données reçues.
                readBuffer.getUByte(b);
                std::cout << (int) b << " ";
            }
            std::cout << std::endl;
            readBuffer.compact();
        }
    } catch (const ClientSocket::SocketReadException &e) {
        Logger::info() << "/" << socket->getIP() << ":" << socket->getPort() << " s'est déconnecté." << std::endl;
        close();
    }
}

void PlayerConnection::runWrite() {
    try {
        // Rien à faire pour le moment.
    } catch (const ClientSocket::SocketWriteException &e) {
        Logger::info() << "/" << socket->getIP() << ":" << socket->getPort() << " s'est déconnecté." << std::endl;
        close();
    }
}
