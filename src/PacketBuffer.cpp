#include "PacketBuffer.h"

#include <arpa/inet.h>
#include <cstring>

PacketBuffer::PacketBuffer() {
    buffer = ubytes_t(BUFFER_SIZE);
    position = limit = mark = 0;
}

void PacketBuffer::get(ubyte_t *b) {
    if (position == limit)
        throw BufferUnderflowException();
    *b = buffer[position++];
}

void PacketBuffer::get(ubyte_t *bs, size_t size) {
    if (position + size > limit)
        throw BufferUnderflowException();
    std::memcpy(bs, &buffer[position], size);
    position += size;
}

void PacketBuffer::getBool(bool &b) {
    get((ubyte_t*) &b);
}

void PacketBuffer::getByte(byte_t &b) {
    get((ubyte_t*) &b);
}

void PacketBuffer::getUByte(ubyte_t &b) {
    get(&b);
}

void PacketBuffer::getBytes(bytes_t &bs) {
    if (bs.size() > 0)
        get((ubyte_t*) bs.data(), bs.size());
}

void PacketBuffer::getUBytes(ubytes_t &bs) {
    if (bs.size() > 0)
        get(bs.data(), bs.size());
}

void PacketBuffer::getShort(short_t &i) {
    get((ubyte_t*) &i, 2);
    i = ntohs(i);
}

void PacketBuffer::getUShort(ushort_t &i) {
    get((ubyte_t*) &i, 2);
    i = ntohs(i);
}

void PacketBuffer::getInt(int_t &i) {
    get((ubyte_t*) &i, 4);
    i = ntohl(i);
}

void PacketBuffer::getUInt(uint_t &i) {
    get((ubyte_t*) &i, 4);
    i = ntohl(i);
}

void PacketBuffer::getVarInt(varint_t &x) {
    x = 0;
    ubyte_t b;
    int shift = 0;
    do {
        get(&b);
        x |= (b & 127) << shift;
        shift += 7;
    } while (b >> 7);
}

void PacketBuffer::getLong(long_t &i) {
    get((ubyte_t*) &i, 8);
    i = (long_t) ntohl(i & 0xffffffff) << 32 | ntohl(i >> 32);
}

void PacketBuffer::getULong(ulong_t &i) {
    get((ubyte_t*) &i, 8);
    i = (long_t) ntohl(i & 0xffffffff) << 32 | ntohl(i >> 32);
}

void PacketBuffer::getFloat(float_t &f) {
    getUInt(*((uint_t*) &f));
}

void PacketBuffer::getDouble(double_t &d) {
    getULong(*((ulong_t*) &d));
}

void PacketBuffer::getString(string_t &s) {
    uint_t size;
    getVarInt(size);
    ubytes_t bs(size);
    getUBytes(bs);
    s = string_t((char*) bs.data(), size);
}

void PacketBuffer::put(ubyte_t *b) {
    if (limit == buffer.size())
        setSize(buffer.size() + 1);
    buffer[position++] = *b;
    if (position > limit)
        limit = position;
}

void PacketBuffer::put(ubyte_t *bs, size_t size) {
    if (limit + size > buffer.size())
        setSize(limit + size);
    std::memcpy(&buffer[position], bs, size);
    position += size;
    if (position > limit)
        limit = position;
}

void PacketBuffer::putBool(bool b) {
    put((ubyte_t*) &b);
}

void PacketBuffer::putByte(byte_t b) {
    put((ubyte_t*) &b);
}

void PacketBuffer::putUByte(ubyte_t b) {
    put(&b);
}

void PacketBuffer::putBytes(bytes_t bs) {
    if (bs.size() > 0)
        put((ubyte_t*) bs.data(), bs.size());
}

void PacketBuffer::putUBytes(ubytes_t bs) {
    if (bs.size() > 0)
        put(bs.data(), bs.size());
}

void PacketBuffer::putShort(short_t i) {
    i = htons(i);
    put((ubyte_t*) &i, 2);
}

void PacketBuffer::putUShort(ushort_t i) {
    i = htons(i);
    put((ubyte_t*) &i, 2);
}

void PacketBuffer::putInt(int_t i) {
    i = htonl(i);
    put((ubyte_t*) &i, 4);
}

void PacketBuffer::putUInt(uint_t i) {
    i = htonl(i);
    put((ubyte_t*) &i, 4);
}

void PacketBuffer::putVarInt(varint_t x) {
    ubyte_t b;
    do {
        b = (x & 127) | ((x > 127) ? 128 : 0);
        put(&b);
        x >>= 7;
    } while (x > 0);
}

void PacketBuffer::putLong(long_t i) {
    i = (long_t) htonl(i & 0xffffffff) << 32 | htonl(i >> 32);
    put((ubyte_t*) &i, 8);
}

void PacketBuffer::putULong(ulong_t i) {
    i = (long_t) htonl(i & 0xffffffff) << 32 | htonl(i >> 32);
    put((ubyte_t*) &i, 8);
};

void PacketBuffer::putFloat(float_t f) {
    putUInt(*((uint_t*) &f));
}

void PacketBuffer::putDouble(double_t d) {
    putULong(*((ulong_t*) &d));
}

void PacketBuffer::putString(string_t s) {
    putVarInt(s.size());
    putUBytes(ubytes_t(s.begin(), s.end()));
}

void PacketBuffer::clear() {
    position = limit = mark = 0;
}

void PacketBuffer::reserve(size_t size) {
    if (size > buffer.size())
        setSize(size);
}

ubyte_t *PacketBuffer::getArray() {
    return buffer.data();
}

size_t PacketBuffer::getLimit() {
    return limit;
}

size_t PacketBuffer::getMark() {
    return mark;
}

size_t PacketBuffer::getPosition() {
    return position;
}

size_t PacketBuffer::getSize() {
    return buffer.size();
}

void PacketBuffer::setLimit(size_t limit) {
    if (limit > buffer.size())
        setSize(limit);
    this->limit = limit;
}

void PacketBuffer::setMark(size_t mark) {
    this->mark = mark;
}

void PacketBuffer::setPosition(size_t position) {
    if (position > limit)
        setLimit(position);
    this->position = position;
}

void PacketBuffer::setSize(size_t size) {
    buffer.resize(((size - 1) / BUFFER_SIZE + 1) * BUFFER_SIZE);
}
