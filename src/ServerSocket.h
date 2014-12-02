#ifndef __Proxy__ServerSocket__
#define __Proxy__ServerSocket__

#include "ClientSocket.h"
#include "Socket.h"

class ServerSocket : public Socket {
public:
    struct SocketAcceptException : public SocketException {
        SocketAcceptException(int code) : SocketException(code) {}
    };

    struct SocketBindException : public SocketException {
        SocketBindException(int code) : SocketException(code) {}
    };

    struct SocketCreateException : public SocketException {
        SocketCreateException(int code) : SocketException(code) {}
    };

    struct SocketListenException : public SocketException {
        SocketListenException(int code) : SocketException(code) {}
    };

    struct SocketSetSockOptException : public SocketException {
        SocketSetSockOptException(int code) : SocketException(code) {}
    };

    ServerSocket(SocketAddress);

    ~ServerSocket();

    virtual void open();

    ClientSocket *accept();

private:
    bool opened;
    bool stage1;
    bool stage2;
};

#endif /* defined(__Proxy__ServerSocket__) */
