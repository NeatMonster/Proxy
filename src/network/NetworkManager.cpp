#include "NetworkManager.h"

#include "Encryption.h"
#include "Logger.h"
#include "Proxy.h"
#include "ServerSocket.h"

NetworkManager::NetworkManager() {
    Encryption::initialize();
    thread = std::thread(&NetworkManager::run, this);
}

NetworkManager::~NetworkManager() {
    for (PlayerConnection *connect : connects) {
        connect->close();
        delete connect;
    }
}

std::vector<PlayerConnection*> NetworkManager::getConnections() {
    for (auto connect = connects.end(); connect != connects.begin();) {
        connect--;
        if ((*connect)->isClosed()) {
            (*connect)->join();
            delete *connect;
            connect = connects.erase(connect);
        }
    }
    return connects;
}

void NetworkManager::run() {
    string_t ip = Proxy::getConfig()->getProxyIP();
    ushort port = Proxy::getConfig()->getProxyPort();
    try {
        ServerSocket *socket = new ServerSocket(Socket::SocketAddress(ip, port));
        socket->open();
        Logger() << "Démarrage du proxy sur " << ip << ":" << port << std::endl;
        while (true) {
            try {
                ClientSocket *clientSocket = socket->accept();
                Logger() << "<" << "/" << clientSocket->getIP() << ":"
                    << clientSocket->getPort() << " <-> Proxy> s'est connecté" << std::endl;
                connects.push_back(new PlayerConnection(clientSocket));
            } catch (const ServerSocket::SocketAcceptException &e) {}
        }
    } catch (const ServerSocket::SocketBindException &e) {
        Logger(LogLevel::WARNING) << "IMPOSSIBLE DE SE LIER À L'IP ET AU PORT !" << std::endl;
        Logger(LogLevel::WARNING) << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger(LogLevel::WARNING) << "Peut-être qu'un serveur occupe déjà ce port ?" << std::endl;
    }
}
