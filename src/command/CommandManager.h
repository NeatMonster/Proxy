#ifndef __Proxy__CommandManager__
#define __Proxy__CommandManager__

#include "Command.h"
#include "CommandQueue.h"
#include "CommandReader.h"
#include "CommandSender.h"
#include "Types.h"

#include <map>

class CommandManager {
public:
    CommandManager();

    ~CommandManager();

    void registerCommand(Command *command);

    void processCommand(string_t, CommandSender*);

    void performHelp(CommandSender*);

    void handleCommands();

private:
    std::map<string_t, Command*> commands;
    CommandReader *reader;
    CommandQueue queue;
};

#endif /* defined(__Proxy__CommandManager__) */
