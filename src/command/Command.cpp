#include "Command.h"

Command::Command(string_t name, string_t description, std::vector<string_t> aliases) :
    name(name), description(description), aliases(aliases) {};

Command::~Command() {}

string_t Command::getName() {
    return name;
}

string_t Command::getDescription() {
    return description;
}

std::vector<string_t > Command::getAliases() {
    return std::vector<string_t>();
}
