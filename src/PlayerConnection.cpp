#include "PlayerConnection.h"

PlayerConnection::PlayerConnection(ClientSocket *socket) : socket(socket) {
    readThread = std::thread(&PlayerConnection::runRead, this);
    writeThread = std::thread(&PlayerConnection::writeThread, this);
}

PlayerConnection::~PlayerConnection() {
    delete socket;
}

void PlayerConnection::runRead() {

}

void PlayerConnection::runWrite() {

}
