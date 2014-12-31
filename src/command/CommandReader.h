#ifndef __Proxy__CommandReader__
#define __Proxy__CommandReader__

#include "CommandQueue.h"

#include <thread>

class CommandReader {
public:
    CommandReader(CommandQueue*);

    ~CommandReader();

private:
    bool running;
    std::thread thread;
    CommandQueue *commands;

    void run();

    int kbhit();
};

#endif /* defined(__Proxy__CommandReader__) */
