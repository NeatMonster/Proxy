#ifndef __Proxy__CommandSender__
#define __Proxy__CommandSender__

#include "ChatMessage.h"
#include "Types.h"

class CommandSender {
public:
    virtual string_t getName() = 0;

    virtual void sendMessage(ChatMessage&) = 0;
};

#endif /* defined(__Proxy__CommandSender__) */
