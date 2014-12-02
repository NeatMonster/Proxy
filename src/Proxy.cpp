#include "Proxy.h"

#include "ServerSocket.h"

int main(void) {
    new Proxy();
    return 0;
}

Proxy::Proxy() {
    run();
}

void Proxy::run() {
    // Pour l'instant, on affiche simplement les clients qui se connectent.
    try {
        ServerSocket *socket = new ServerSocket(Socket::SocketAddress("127.0.0.1", 25565));
        socket->open();
        while (true) {
            ClientSocket *clientSocket = socket->accept();
            std::cout << "/" << clientSocket->getIP() << ":" << clientSocket->getPort() << " s'est connecté." << std::endl;
        }
    } catch (const ServerSocket::SocketBindException &e) {
        std::cout << "Le port 25565 est déjà occupé." << std::endl;
    }
}
