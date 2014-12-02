#include "Proxy.h"

#include "Logger.h"
#include "ServerSocket.h"

int main() {
    new Proxy();
    return 0;
}

Proxy::Proxy() {
    run();
}

Proxy::~Proxy() {
    for (PlayerConnection *connect : connects)
        delete connect;
}

void Proxy::run() {
    // Pour l'instant, on affiche simplement les clients qui se connectent.
    string_t ip = "127.0.0.1";
    ushort port = 25565;
    try {
        ServerSocket *socket = new ServerSocket(Socket::SocketAddress(ip, port));
        socket->open();
        Logger::info() << "Démarrage du Proxy sur " << ip << ":" << port << std::endl;
        while (true) {
            try {
                ClientSocket *clientSocket = socket->accept();
                Logger::info() << "/" << clientSocket->getIP() << ":" <<
                    clientSocket->getPort() << " s'est connecté." << std::endl;
                connects.push_back(new PlayerConnection(clientSocket));
            } catch (const ServerSocket::SocketAcceptException &e) {}
        }
    } catch (const ServerSocket::SocketBindException &e) {
        Logger::warning() << "IMPOSSIBLE DE SE LIER À L'IP ET AU PORT !" << std::endl;
        Logger::warning() << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger::warning() << "Peut-être qu'un serveur occupe déjà ce port ?" << std::endl;
    }
}
