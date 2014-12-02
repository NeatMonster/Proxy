#include "ClientSocket.h"

#include <errno.h>

ClientSocket::ClientSocket(int handle, SocketAddress address) : Socket(address) {
    this->handle = handle;
    readPointer = readBuffer;
    dataLength = 0;
}

ClientSocket::~ClientSocket() {
    try {
        if (opened)
            close();
    } catch (...) {}
}

void ClientSocket::open() {}

size_t ClientSocket::transmit(ubyte_t *buffer, size_t size) {
    if (buffer == nullptr)
        throw InvalidArgumentException("Buffer invalide.");
    else if (size == 0)
        return 0;
    int count = ::send(handle, buffer, size, MSG_NOSIGNAL);
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
        dataLength = ::recv(handle, readBuffer, sizeof(readBuffer), MSG_NOSIGNAL);
        if (dataLength <= 0) {
            if (errno != EWOULDBLOCK)
                throw SocketReadException(errno);
        }
    }
    if (dataLength < (int) size)
        size = dataLength;
    int remaining = size;
    while (remaining > 0) {
        *buffer = *readPointer;
        buffer++;
        readPointer++;
        dataLength--;
        remaining--;
    }
    return size;
}

string_t ClientSocket::getIP() {
    return address.getIP();
}

ushort_t ClientSocket::getPort() {
    return address.getPort();
}
