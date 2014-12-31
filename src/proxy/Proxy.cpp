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

NetworkManager *Proxy::getNetwork() {
    return instance->network;
}

Profile &Proxy::getProfile(string_t uuid) {
    return instance->uuids[uuid];
}

void Proxy::addProfile(string_t uuid, Profile profile) {
    instance->uuids[uuid] = profile;
}

Proxy::Proxy() : running(true) {
    instance = this;
    Logger() << "Démarrage du proxy" << std::endl;
    commands = new CommandManager();
    config = new ConfigManager();
    network = new NetworkManager();
    if (network->start()) {
        run();
        network->stop();
    }
}

Proxy::~Proxy() {
    delete commands;
    delete config;
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
