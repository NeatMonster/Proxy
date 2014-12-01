#ifndef __Proxy__ClientSocket__
#define __Proxy__ClientSocket__

#include "Socket.h"

class ClientSocket : public Socket {
public:
    struct SocketReadException : public SocketException {
        SocketReadException(int code) : SocketException(code) {};
    };

    struct SocketWriteException : public SocketException {
        SocketWriteException(int code) : SocketException(code) {};
    };

    ClientSocket(int, SocketAddress);

    ~ClientSocket();

    virtual void open();

    virtual int transmit(char*, int);

    virtual int receive(char*, int);

    string_t getIP();

    ushort_t getPort();

private:
    char readBuffer[4096];
    char* readPointer;
    size_t dataLength;
};

#endif /* defined(__Proxy__ClientSocket__) */
