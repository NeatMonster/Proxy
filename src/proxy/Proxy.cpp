#include "Proxy.h"

#include "Logger.h"
#include "ServerSocket.h"

#include <chrono>
#include <thread>

int main() {
    delete new Proxy();
    return 0;
}

Proxy *Proxy::getProxy() {
    return instance;
}

ConfigManager *Proxy::getConfig() {
    return instance->config;
}

Database *Proxy::getDatabase() {
    return instance->database;
}

NetworkManager *Proxy::getNetwork() {
    return instance->network;
}

Proxy::Proxy() : running(true) {
    instance = this;
    Logger() << "Démarrage du proxy" << std::endl;
    commands = new CommandManager();
    config = new ConfigManager();
    database = new Database();
    network = new NetworkManager();
    if (database->run() && network->start()) {
        run();
        network->stop();
    }
}

Proxy::~Proxy() {
    delete commands;
    delete config;
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
