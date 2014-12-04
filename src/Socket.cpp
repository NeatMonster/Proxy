#include "Socket.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Socket::SocketAddress::SocketAddress(string_t ip, ushort_t port) {
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
}

string_t Socket::SocketAddress::getIP() {
    ulong_t ip = ntohl(address.sin_addr.s_addr);
    string_t s;
    s += std::to_string((ip >> 24) & 0x000000ff) + ".";
    s += std::to_string((ip >> 16) & 0x000000ff) + ".";
    s += std::to_string((ip >> 8) & 0x000000ff) + ".";
    s += std::to_string(ip & 0x000000ff);
    return s;
}

ushort_t Socket::SocketAddress::getPort() {
    return ntohs(address.sin_port);
}

struct sockaddr *Socket::SocketAddress::getAddress() {
    return (struct sockaddr*) &address;
}

socklen_t Socket::SocketAddress::getSize() {
    return sizeof(address);
}

Socket::Socket(SocketAddress address) : address(address), handle(INVALID_HANDLE), opened(false) {}

Socket::~Socket() {
    if (handle > 0) {
        ::shutdown(handle, SHUT_RD);
        ::close(handle);
    }
}

bool Socket::isOpen() {
    return handle != INVALID_HANDLE && opened;
}

void Socket::close() {
    if (handle > 0) {
        ::shutdown(handle, SHUT_RD);
        if (::close(handle) && errno != EWOULDBLOCK)
            throw SocketCloseException(errno);
        handle = INVALID_HANDLE;
        opened = false;
    }
}

