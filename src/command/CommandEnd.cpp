#include "CommandEnd.h"

#include "Proxy.h"

CommandEnd::CommandEnd() : Command("end", "Eteins le proxy", {}) {};

void CommandEnd::perform(CommandSender*, std::vector<string_t>) {
    Proxy::getProxy()->stop();
}
