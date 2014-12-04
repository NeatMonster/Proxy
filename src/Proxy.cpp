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

NetworkManager *Proxy::getNetwork() {
    return getProxy()->network;
}

Proxy::Proxy() {
    instance = this;
    run();
}

Proxy::~Proxy() {
    delete network;
}

Proxy *Proxy::instance;

void Proxy::run() {
    network = new NetworkManager();
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}
