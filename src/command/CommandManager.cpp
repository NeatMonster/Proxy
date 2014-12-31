#include "CommandManager.h"

#include "ChatMessage.h"
#include "CommandEnd.h"
#include "Proxy.h"

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

CommandManager::CommandManager() {
    registerCommand(new CommandEnd());
};

CommandManager::~CommandManager() {
    for (auto command : commands)
        delete command.second;
};

void CommandManager::registerCommand(Command *command) {
    commands[command->getName()] = command;
    for (string_t alias : command->getAliases())
        commands[alias] = command;
}

void CommandManager::processCommand(string_t s, CommandSender *sender) {
    string_t arg;
    std::vector<string_t> args;
    std::istringstream ss(s, std::istringstream::in);
    while (ss >> arg)
        args.push_back(arg);
    if (args.empty())
        return;
    string_t name = args[0];
    args.erase(args.begin());
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    if (name == "help")
        performHelp(sender);
    else if (commands.find(name) == commands.end())
        sender->sendMessage(Chat() << "Commande inconnue. Essayez /help pour une liste des commandes.");
    else
        try {
            commands[name]->perform(sender, args);
        } catch (const Command::WrongUsageException &e) {
            sender->sendMessage(Chat() << Color::RED << "Usage : " << e.what());
        } catch (const Command::CommandException &e) {
            sender->sendMessage(Chat() << Color::RED << "Erreur : " << e.what());
        }
}

void CommandManager::performHelp(CommandSender *sender) {
    //TODO Gérer plusieurs pages
    sender->sendMessage(Chat() << "--- Affichage de la page 1 sur 1 ---");
    for (std::map<string_t,Command*>::iterator it = commands.begin(); it != commands.end(); it++)
        sender->sendMessage(Chat() << ChatMessage::Style::BOLD << it->second->getName()
                            << ChatMessage::Style::RESET << " : " << it->second->getDescription());
}

void CommandManager::handleCommands() {
    string_t command;
    if (kbhit() > 0) {
        std::getline(std::cin, command);
        processCommand(command, Proxy::getProxy());
    }
}

int CommandManager::kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
