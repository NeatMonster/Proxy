#include "ClientSocket.h"

#include <errno.h>

ClientSocket::ClientSocket(SocketAddress address) : Socket(address) {
    if ((handle = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_HANDLE)
        throw SocketCreateException(errno);
    readPointer = readBuffer;
    dataLength = 0;
}

ClientSocket::ClientSocket(int handle, SocketAddress address) : Socket(address) {
    this->handle = handle;
    opened = true;
    readPointer = readBuffer;
    dataLength = 0;
}

ClientSocket::~ClientSocket() {
    try {
        if (isOpen())
            close();
    } catch (...) {}
}

void ClientSocket::open() {
    if (!isOpen()) {
        if (::connect(handle, address.getAddress(), address.getSize()) < 0)
            throw SocketConnectException(errno);
        opened = true;
    }
}

size_t ClientSocket::transmit(ubyte_t *buffer, size_t size) {
    if (buffer == nullptr)
        throw InvalidArgumentException("Buffer invalide.");
    else if (size == 0)
        return 0;
    int count = ::send(handle, buffer, size, 0);
    if (count < 0) {
        if (errno == EWOULDBLOCK)
            return 0;
        else
            throw SocketWriteException(errno);
    }
    return count;
}

size_t ClientSocket::receive(ubyte_t *buffer, size_t size) {
    if (dataLength <= 0) {
        readPointer = readBuffer;
        dataLength = ::recv(handle, readBuffer, sizeof(readBuffer), 0);
        if (dataLength <= 0) {
            if (errno != EWOULDBLOCK)
                throw SocketReadException(errno);
        }
    }
    if (dataLength < (int) size)
        size = dataLength;
    int remaining = size;
    while (remaining-- > 0) {
        *(buffer++) = *(readPointer++);
        dataLength--;
    }
    return size;
}

string_t ClientSocket::getIP() {
    return address.getIP();
}

ushort_t ClientSocket::getPort() {
    return address.getPort();
}
