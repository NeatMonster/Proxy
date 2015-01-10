#include "Proxy.h"

#include "Logger.h"
#include "ServerSocket.h"
#include "Types.h"

#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    ushort_t port = 25565;
    for (int i = 1; i < argc; i++) {
        string_t arg = string_t(argv[i]);
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage : " << string_t(argv[0]) << " [options]" << std::endl;
            std::cout << "Options : " << std::endl;
            std::cout << "\t-h,--help\t\tAffiche l'aide" << std::endl;
            std::cout << "\t-p,--port\t\tSpécifie le port" << std::endl;
            return 0;
        } else if (arg == "-p" || arg == "--port") {
            try {
                port = std::stoi(string_t(argv[++i]));
            } catch (const std::exception &e) {
                std::cout << "Le port spécifié est invalide" << std::endl;
                return 0;
            }
        } else {
            std::cout << "Option '" << arg << "' non reconnue." << std::endl;
            return 0;
        }
    }
    delete new Proxy(port);
    return 0;
}

Proxy *Proxy::getProxy() {
    return instance;
}

Database *Proxy::getDatabase() {
    return instance->database;
}

NetworkManager *Proxy::getNetwork() {
    return instance->network;
}

Proxy::Proxy(ushort_t port) : running(true) {
    instance = this;
    Logger() << "Démarrage du proxy" << std::endl;
    commands = new CommandManager();
    database = new Database();
    network = new NetworkManager();
    if (database->run() && network->start(port)) {
        run();
        network->stop();
    }
}

Proxy::~Proxy() {
    delete commands;
    delete database;
    delete network;
}

void Proxy::stop() {
    running = false;
    Logger() << "Extinction du proxy" << std::endl;
}

string_t Proxy::getName() {
    return "Proxy";
}

void Proxy::sendMessage(ChatMessage &message) {
    Logger() << message.getText() << std::endl;
}

Proxy *Proxy::instance;

void Proxy::run() {
    while (running) {
        network->cleanup();
        commands->handleCommands();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
