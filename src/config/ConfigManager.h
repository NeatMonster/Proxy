#ifndef __Proxy__ConfigManager__
#define __Proxy__ConfigManager__

#include "Types.h"

#include <map>

class ConfigManager {
public:
    ConfigManager();

    void load();

    void save();

    string_t getProxyIP();

    int_t getProxyPort();

    string_t getDefaultServer();

    std::map<string_t, std::pair<string_t, int_t>> getServers();

private:
    string_t proxyIP = "0.0.0.0";
    int_t proxyPort = 25565;
    string_t defaultServer = "lobby";
    std::map<string_t, std::pair<string_t, int_t>> servers = {
        {"lobby", {"0.0.0.0", 25566}},
        {"game", {"0.0.0.0", 25567}}
    };
};

#endif /* defined(__Proxy__ConfigManager__) */
