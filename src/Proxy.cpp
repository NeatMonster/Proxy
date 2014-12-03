#include "Proxy.h"

#include "Logger.h"
#include "ServerSocket.h"

#include <chrono>
#include <thread>

int main() {
    new Proxy();
    return 0;
}

Proxy::Proxy() {
    run();
}

Proxy::~Proxy() {
    delete network;
}

void Proxy::run() {
    network = new NetworkManager();
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}
