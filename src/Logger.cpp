#include "Logger.h"

#include <iomanip>

#include <chrono>
#include <fstream>

#define TIMEZONE 1

using namespace std::chrono;
typedef std::chrono::system_clock Clock;

Logger::Logger(Logger::Level level) {
    lock = std::unique_lock<std::mutex>(mutex);
    Clock::duration time = Clock::now().time_since_epoch();
    string_t h = std::to_string((duration_cast<hours>(time).count() + TIMEZONE) % 24);
    string_t m = std::to_string(duration_cast<minutes>(time).count() % 60);
    string_t s = std::to_string(duration_cast<seconds>(time).count() % 60);
    *this << "[" << (h.length() == 1 ? "0" + h : h);
    *this << ":" << (m.length() == 1 ? "0" + m : m);
    *this << ":" << (s.length() == 1 ? "0" + s : s);
    switch (level) {
        case Level::INFO:
            *this << " INFO] ";
            break;
        case Level::WARNING:
            *this << " WARNING] ";
            break;
        case Level::SEVERE:
            *this << " SEVERE] ";
            break;
        case Level::DEBUG:
            *this << " DEBUG] ";
            break;
    }
}

Logger::~Logger() {
    std::cout << str();
    std::ofstream logFile("log.txt", std::ios::app | std::ios::ate);
    logFile << str();
    logFile.close();
    lock.unlock();
}

std::mutex Logger::mutex;