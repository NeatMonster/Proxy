#include "ServerSocket.h"

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_PENDING 50

ServerSocket::ServerSocket(SocketAddress address) : Socket(address), reuse(false), bound(false) {
    if ((handle = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_HANDLE)
        throw SocketCreateException(errno);
}

ServerSocket::~ServerSocket() {
    try {
        if (isOpen())
            close();
    } catch (...) {}
}

void ServerSocket::open() {
    if (!opened) {
        if (!reuse) {
            int tmp = true;
            if (::setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*) &tmp, sizeof(tmp)) < 0)
                throw SocketSetSockOptException(errno);
            reuse = true;
        }
        if (!bound) {
            if (::bind(handle, address.getAddress(), address.getSize()) < 0)
                throw SocketBindException(errno);
            bound = true;
        }
        if (::listen(handle, MAX_PENDING) < 0)
            throw SocketListenException(errno);
        opened = true;
    }
}

ClientSocket *ServerSocket::accept() {
    socklen_t size = address.getSize();
    int newHandle = ::accept(handle, address.getAddress(), &size);
    if (newHandle == -1) {
        if (errno == EWOULDBLOCK)
            return nullptr;
        else
            throw SocketAcceptException(errno);
    }
    if (SO_NOSIGPIPE != 0 && MSG_NOSIGNAL == 0) {
        int tmp = true;
        if (::setsockopt(newHandle, SOL_SOCKET, SO_NOSIGPIPE, (char*) &tmp, sizeof(tmp)) < 0) {
            ::shutdown(newHandle, SHUT_RD);
            ::close(newHandle);
            throw SocketSetSockOptException(errno);
        }
    }
    return new ClientSocket(newHandle, address);
}
