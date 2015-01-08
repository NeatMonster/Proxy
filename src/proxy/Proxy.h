#ifndef __Proxy__Proxy__
#define __Proxy__Proxy__

#include "ChatMessage.h"
#include "CommandManager.h"
#include "CommandSender.h"
#include "ConfigManager.h"
#include "Database.h"
#include "NetworkManager.h"

#include <unordered_map>

class Proxy : public CommandSender {
public:
    static Proxy *getProxy();

    static ConfigManager *getConfig();

    static CommandManager *getCommands();

    static Database *getDatabase();

    static NetworkManager *getNetwork();

    Proxy();

    virtual ~Proxy();

    void stop();

    string_t getName();

    void sendMessage(ChatMessage&);

private:
    static Proxy *instance;
    CommandManager *commands;
    ConfigManager *config;
    Database *database;
    NetworkManager *network;
    bool running;

    void run();
};

#endif /* defined(__Proxy__Proxy__) */
