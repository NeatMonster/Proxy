#include "NetworkManager.h"

#include "Encryption.h"
#include "Logger.h"
#include "Proxy.h"

NetworkManager::NetworkManager() : running(false) {
    Encryption::initialize();
}

NetworkManager::~NetworkManager() {
    for (PlayerConnection *connect : connects)
        delete connect;
}

bool NetworkManager::start() {
    string_t ip = Proxy::getConfig()->getProxyIP();
    ushort_t port = Proxy::getConfig()->getProxyPort();
    try {
        socket = new ServerSocket(Socket::SocketAddress(ip, port));
        socket->open();
        Logger() << "Démarrage du proxy sur " << ip << ":" << port << std::endl;
        running = true;
        thread = std::thread(&NetworkManager::run, this);
        return true;
    } catch (const ServerSocket::SocketBindException &e) {
        Logger(LogLevel::WARNING) << "IMPOSSIBLE DE SE LIER À L'IP ET AU PORT !" << std::endl;
        Logger(LogLevel::WARNING) << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger(LogLevel::WARNING) << "Peut-être qu'un serveur occupe déjà ce port ?" << std::endl;
    }
    return false;
}

bool NetworkManager::stop() {
    for (PlayerConnection *&connect : connects) {
        connect->disconnect("Proxy fermé");
        connect->join();
    }
    running = false;
    socket->close();
    thread.join();
    return true;
}

void NetworkManager::cleanup() {
    for (auto connect = connects.end(); connect != connects.begin();) {
        connect--;
        if ((*connect)->isClosed()) {
            (*connect)->join();
            delete *connect;
            connect = connects.erase(connect);
        }
    }
}

void NetworkManager::run() {
    while (running)
        try {
            ClientSocket *clientSocket = socket->accept();
            Logger() << "<" << "/" << clientSocket->getIP() << ":"
                << clientSocket->getPort() << " <-> Proxy> s'est connecté" << std::endl;
            connects.push_back(new PlayerConnection(clientSocket));
        } catch (const ServerSocket::SocketAcceptException &e) {}
}
