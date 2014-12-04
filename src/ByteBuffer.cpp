#include "ByteBuffer.h"

#include <cstring>

ByteBuffer::ByteBuffer() {
    buffer = ubytes_t(BUFFER_SIZE);
    position = limit = 0;
}

void ByteBuffer::get(ubyte_t *b) {
    if (position == limit)
        throw BufferUnderflowException();
    *b = buffer[position++];
}

void ByteBuffer::get(ubyte_t *bs, size_t size) {
    if (position + size > limit)
        throw BufferUnderflowException();
    std::memcpy(bs, &buffer[position], size);
    position += size;
}

void ByteBuffer::getBool(bool &b) {
    get((ubyte_t*) &b);
}

void ByteBuffer::getByte(byte_t &b) {
    get((ubyte_t*) &b);
}

void ByteBuffer::getUByte(ubyte_t &b) {
    get(&b);
}

void ByteBuffer::getBytes(bytes_t &bs) {
    if (bs.size() > 0)
        get((ubyte_t*) bs.data(), bs.size());
}

void ByteBuffer::getUBytes(ubytes_t &bs) {
    if (bs.size() > 0)
        get(bs.data(), bs.size());
}

void ByteBuffer::getShort(short_t &i) {
    ubyte_t bs[2];
    get(bs, 2);
    reverse(bs, 2);
    std::memcpy(&i, bs, 2);
}

void ByteBuffer::getUShort(ushort_t &i) {
    ubyte_t bs[2];
    get(bs, 2);
    reverse(bs, 2);
    std::memcpy(&i, bs, 2);
}

void ByteBuffer::getInt(int_t &i) {
    ubyte_t bs[4];
    get(bs, 4);
    reverse(bs, 4);
    std::memcpy(&i, bs, 4);
}

void ByteBuffer::getUInt(uint_t &i) {
    ubyte_t bs[4];
    get(bs, 4);
    reverse(bs, 4);
    std::memcpy(&i, bs, 4);
}

void ByteBuffer::getVarInt(varint_t &x) {
    x = 0;
    ubyte_t b = 0;
    int shift = 0;
    do {
        get(&b);
        x |= (b & 127) << shift;
        shift += 7;
    } while (b >> 7);
}

void ByteBuffer::getLong(long_t &i) {
    ubyte_t bs[8];
    get(bs, 8);
    reverse(bs, 8);
    std::memcpy(&i, bs, 8);
}

void ByteBuffer::getULong(ulong_t &i) {
    ubyte_t bs[8];
    get(bs, 8);
    reverse(bs, 8);
    std::memcpy(&i, bs, 8);
}

void ByteBuffer::getFloat(float_t &f) {
    ubyte_t bs[4];
    get(bs, 4);
    reverse(bs, 4);
    std::memcpy(&f, bs, 4);
}

void ByteBuffer::getDouble(double_t &d) {
    ubyte_t bs[8];
    get(bs, 8);
    reverse(bs, 8);
    std::memcpy(&d, bs, 8);
}

void ByteBuffer::getString(string_t &s) {
    uint_t size;
    getVarInt(size);
    ubytes_t bs(size);
    getUBytes(bs);
    s = string_t((char*) bs.data(), size);
}

void ByteBuffer::put(ubyte_t *b) {
    if (limit == buffer.size())
        setSize(buffer.size() + 1);
    buffer[position++] = *b;
    if (position > limit)
        limit = position;
}

void ByteBuffer::put(ubyte_t *bs, size_t size) {
    if (limit + size > buffer.size())
        setSize(limit + size);
    std::memcpy(&buffer[position], bs, size);
    position += size;
    if (position > limit)
        limit = position;
}

void ByteBuffer::putBool(bool b) {
    put((ubyte_t*) &b);
}

void ByteBuffer::putByte(byte_t b) {
    put((ubyte_t*) &b);
}

void ByteBuffer::putUByte(ubyte_t b) {
    put(&b);
}

void ByteBuffer::putBytes(bytes_t bs) {
    if (bs.size() > 0)
        put((ubyte_t*) bs.data(), bs.size());
}

void ByteBuffer::putUBytes(ubytes_t bs) {
    if (bs.size() > 0)
        put(bs.data(), bs.size());
}

void ByteBuffer::putShort(short_t i) {
    ubyte_t bs[2];
    std::memcpy(bs, &i, 2);
    reverse(bs, 2);
    put(bs, 2);
}

void ByteBuffer::putUShort(ushort_t i) {
    ubyte_t bs[2];
    std::memcpy(bs, &i, 2);
    reverse(bs, 2);
    put(bs, 2);
}

void ByteBuffer::putInt(int_t i) {
    ubyte_t bs[4];
    std::memcpy(bs, &i, 4);
    reverse(bs, 4);
    put(bs, 4);
}

void ByteBuffer::putUInt(uint_t i) {
    ubyte_t bs[4];
    std::memcpy(bs, &i, 4);
    reverse(bs, 4);
    put(bs, 4);
}

void ByteBuffer::putVarInt(varint_t x) {
    size_t size = ::getSize(x);
    ubyte_t bs[5];
    for (size_t i = 0; i < size; i++) {
        bs[i] = (x & 127) | ((x > 127) ? 128 : 0);
        x >>= 7;
    }
    put(bs, size);
}

void ByteBuffer::putLong(long_t i) {
    ubyte_t bs[8];
    std::memcpy(bs, &i, 8);
    reverse(bs, 8);
    put(bs, 8);
}

void ByteBuffer::putULong(ulong_t i) {
    ubyte_t bs[8];
    std::memcpy(bs, &i, 8);
    reverse(bs, 8);
    put(bs, 8);
};

void ByteBuffer::putFloat(float_t f) {
    ubyte_t bs[4];
    std::memcpy(bs, &f, 4);
    reverse(bs, 4);
    put(bs, 4);
}

void ByteBuffer::putDouble(double_t d) {
    ubyte_t bs[8];
    std::memcpy(bs, &d, 8);
    reverse(bs, 8);
    put(bs, 8);
}

void ByteBuffer::putString(string_t s) {
    putVarInt(s.size());
    putUBytes(ubytes_t(s.begin(), s.end()));
}

void ByteBuffer::clear() {
    position = limit = 0;
}

void ByteBuffer::compact() {
    if (position < limit)
        memcpy(&buffer[0], &buffer[position], limit - position);
    limit -= position;
    position = 0;
}

ubyte_t *ByteBuffer::getData() {
    return buffer.data();
}

size_t ByteBuffer::getLimit() {
    return limit;
}

size_t ByteBuffer::getPosition() {
    return position;
}

size_t ByteBuffer::getSize() {
    return buffer.size();
}

void ByteBuffer::rewind() {
    position = 0;
}

void ByteBuffer::setLimit(size_t limit) {
    if (limit > buffer.size())
        setSize(limit);
    this->limit = limit;
}

void ByteBuffer::setPosition(size_t position) {
    this->position = position;
}

void ByteBuffer::setSize(size_t size) {
    buffer.resize(((size - 1) / BUFFER_SIZE + 1) * BUFFER_SIZE);
}

void ByteBuffer::shift(size_t shift) {
    if (shift == 0 || position == limit)
        return;
    if (limit + shift > buffer.size())
        setSize(limit + shift);
    for (int i = limit - 1; i >= (int) position; i--)
        buffer[i + shift] = buffer[i];
    limit += shift;
}

void ByteBuffer::reverse(ubyte_t *bs, size_t size) {
    ubyte_t b;
    for (size_t i = 0; i < size / 2; i++) {
        b = bs[i];
        bs[i] = bs[size - i - 1];
        bs[size - i - 1] = b;
    }
}
