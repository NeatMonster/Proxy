#include "ServerSocket.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PENDING 5

ServerSocket::ServerSocket(SocketAddress address) : Socket(address), opened(false), stage1(false), stage2(false) {
    if ((handle = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_HANDLE)
        throw SocketCreateException(errno);
}

ServerSocket::~ServerSocket() {
    try {
        close();
    } catch (...) {}
}

void ServerSocket::open() {
    if (!opened) {
        if (!stage1) {
            int reuseAddress = true;
            if (::setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*) &reuseAddress, sizeof(reuseAddress)) < 0)
                throw SocketSetSockOptException(errno);
            stage1 = true;
        }
        if (!stage2) {
            if (::bind(handle, address.getAddress(), address.getSize()) < 0)
                throw SocketBindException(errno);
            stage2 = true;
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
        int turnedOn = 1;
        if (::setsockopt(newHandle, SOL_SOCKET, SO_NOSIGPIPE, (char*) &turnedOn, sizeof(turnedOn)) < 0) {
            ::shutdown(newHandle, SHUT_RD);
            ::close(newHandle);
            throw SocketSetSockOptException(errno);
        }
    }
    return new ClientSocket(newHandle, address);
}
