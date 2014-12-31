#include "Proxy.h"

#include "Logger.h"
#include "ServerSocket.h"

#include <chrono>
#include <thread>

int main() {
    new Proxy();
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

Proxy::Proxy() {
    instance = this;
    run();
}

Proxy::~Proxy() {
    delete commands;
    delete config;
    delete network;
}

void Proxy::stop() {
    //TODO Implémenter l'extinction
}

string_t Proxy::getName() {
    return "Proxy";
}

void Proxy::sendMessage(ChatMessage &message) {
    Logger() << message.getText() << std::endl;
}

Proxy *Proxy::instance;

void Proxy::run() {
    commands = new CommandManager();
    config = new ConfigManager();
    network = new NetworkManager();
    while (true) {
        commands->handleCommands();
        //TODO Trouver une autre façon de faire ça
        network->getConnections();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
