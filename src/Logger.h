#ifndef __Proxy__Logger__
#define __Proxy__Logger__

#include "Types.h"

#include <mutex>
#include <sstream>

class Logger : public std::ostringstream {
public:
    enum Level { INFO, WARNING, SEVERE, DEBUG };

    Logger(Level = Level::INFO);

    ~Logger();

private:
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock;
};

typedef Logger::Level LogLevel;

#endif /* defined(__Proxy__Logger__) */
