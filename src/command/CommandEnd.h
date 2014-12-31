#ifndef __Proxy__CommandEnd__
#define __Proxy__CommandEnd__

#include "Command.h"

class CommandEnd : public Command {
public:
    CommandEnd();

    void perform(CommandSender*, std::vector<string_t>);
};

#endif /* defined(__Proxy__CommandEnd__) */
