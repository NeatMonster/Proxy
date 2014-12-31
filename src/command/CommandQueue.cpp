#include "CommandQueue.h"

void CommandQueue::push(string_t command) {
    std::unique_lock<std::mutex> lock(mutex);
    vector.push_back(command);
    lock.unlock();
}

bool CommandQueue::tryPop(string_t *command) {
    std::unique_lock<std::mutex> lock(mutex);
    if (vector.empty())
        return false;
    *command = vector.front();
    vector.pop_back();
    return true;
}
