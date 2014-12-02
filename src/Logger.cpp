#include "Logger.h"
#include <iomanip>

std::ostream &Logger::info() {
    time() << "INFO] ";
    return std::cout;
}

std::ostream &Logger::warning() {
    time() << "WARNING] ";
    return std::cout;
}

std::ostream &Logger::severe() {
    time() << "SEVERE] ";
    return std::cout;
}

std::ostream &Logger::time() {
    time_t now = ::time(0);
    tm *ltm = localtime(&now);
    std::cout << std::setfill('0') << "[" << std::setw(2) << ltm->tm_hour << ":"
        << std::setw(2) << ltm->tm_min << ":" << std::setw(2) << ltm->tm_sec << " ";
    return std::cout;
}
