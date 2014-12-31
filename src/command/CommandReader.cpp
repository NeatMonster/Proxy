#include "CommandReader.h"

#include "Types.h"

#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

CommandReader::CommandReader(CommandQueue *commands) : running(true), commands(commands) {
    thread = std::thread(&CommandReader::run, this);
}

CommandReader::~CommandReader() {
    running = false;
    thread.join();
}

void CommandReader::run() {
    string_t command;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (kbhit() > 0) {
            std::getline(std::cin, command);
            commands->push(command);
        }
    }
}

int CommandReader::kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
