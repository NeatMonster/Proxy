#ifndef __Proxy__Command__
#define __Proxy__Command__

#include "CommandSender.h"
#include "Types.h"

#include <stdexcept>
#include <vector>

class Command {
public:
    class CommandException : public std::runtime_error {
    public:
        CommandException(string_t message) : std::runtime_error(message) {};
    };

    class WrongUsageException : public CommandException {
    public:
        WrongUsageException(string_t message) : CommandException(message) {};
    };

    Command(string_t, string_t, std::vector<string_t>);

    virtual ~Command();

    string_t getName();

    string_t getDescription();

    std::vector<string_t> getAliases();

    virtual void perform(CommandSender*, std::vector<string_t>) = 0;

private:
    string_t name;
    string_t description;
    std::vector<string_t> aliases;
};

#endif /* defined(__Proxy__Command__) */
