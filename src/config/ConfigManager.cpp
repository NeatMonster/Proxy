#include "ConfigManager.h"

#include "json11/json11.hpp"

#include <fstream>
#include <sstream>

ConfigManager::ConfigManager() {
    load();
}

void ConfigManager::load() {
    std::ifstream input("config.json");
    std::stringstream buffer;
    buffer << buffer.rdbuf();
    input.close();
    string_t err;
    json11::Json config = json11::Json::parse(buffer.str(), err);
    if (config["proxyIP"].is_string())
        proxyIP = config["proxyIP"].string_value();
    if (config["proxyPort"].is_number())
        proxyPort = config["proxyPort"].number_value();
    if (config["defaultServer"].is_string())
        defaultServer = config["defaultServer"].string_value();
    if (config["servers"].is_object())
        for (auto server : config["servers"].object_items()) {
            string_t serverName = server.first;
            if (server.second.is_object()) {
                string_t serverIP = server.second.object_items().begin()->first;
                ushort_t serverPort = server.second.object_items().begin()->second.number_value();
                servers[serverName] = std::make_pair(serverIP, serverPort);
            }
        }
    save();
}

void ConfigManager::save() {
    std::ofstream output("config.json");
    std::map<string_t, std::map<string_t, int_t>> servers;
    for (auto server : this->servers)
        servers.insert({server.first, {server.second}});
    json11::Json config = json11::Json::object {
        {"proxyIP", proxyIP},
        {"proxyPort", proxyPort},
        {"defaultServer", defaultServer},
        {"servers", servers}
    };
    output << config.dump();
    output.close();
}

string_t ConfigManager::getProxyIP() {
    return proxyIP;
}

ushort_t ConfigManager::getProxyPort() {
    return proxyPort;
}

string_t ConfigManager::getDefaultServer() {
    return defaultServer;
}

std::map<string_t, std::pair<string_t, ushort_t>> ConfigManager::getServers() {
    return servers;
}
