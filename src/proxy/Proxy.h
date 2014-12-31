#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "ChatMessage.h"
#include "CommandManager.h"
#include "CommandSender.h"
#include "ConfigManager.h"
#include "NetworkManager.h"

#include <unordered_map>

class Proxy : public CommandSender {
public:
    static Proxy *getProxy();

    static ConfigManager *getConfig();

    static CommandManager *getCommands();

    static NetworkManager *getNetwork();

    static Profile &getProfile(string_t);

    static void addProfile(string_t, Profile);

    Proxy();

    ~Proxy();

    void stop();

    string_t getName();

    void sendMessage(ChatMessage&);

private:
    static Proxy *instance;
    CommandManager *commands;
    ConfigManager *config;
    NetworkManager *network;
    std::unordered_map<string_t, Profile> uuids;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
