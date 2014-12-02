#ifndef __Proxy__Logger__
#define __Proxy__Logger__

#include <iostream>

class Logger {
public:
    static std::ostream &info();

    static std::ostream &warning();

    static std::ostream &severe();

private:
    static std::ostream &time();
};

#endif /* defined(__Proxy__Logger__) */
