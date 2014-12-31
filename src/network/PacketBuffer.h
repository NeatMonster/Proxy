#ifndef __Proxy__PacketBuffer__
#define __Proxy__PacketBuffer__

#include "Types.h"

#include <stdexcept>

class PacketBuffer {
public:
    struct BufferUnderflowException : public std::runtime_error {
        BufferUnderflowException() : runtime_error("Pas assez de données.") {}
    };

    PacketBuffer();

    void get(ubyte_t*);

    void get(ubyte_t*, size_t);

    void getBool(bool&);

    void getByte(byte_t&);

    void getUByte(ubyte_t&);

    void getBytes(bytes_t&);

    void getUBytes(ubytes_t&);

    void getShort(short_t&);

    void getUShort(ushort_t&);

    void getInt(int_t&);

    void getUInt(uint_t&);

    void getVarInt(varint_t&);

    void getLong(long_t&);

    void getULong(ulong_t&);

    void getFloat(float_t&);

    void getDouble(double_t&);

    void getString(string_t&);

    void put(ubyte_t*);

    void put(ubyte_t*, size_t);

    void putBool(bool);

    void putByte(byte_t);

    void putUByte(ubyte_t);

    void putBytes(bytes_t);

    void putUBytes(ubytes_t);

    void putShort(short_t);

    void putUShort(ushort_t);

    void putInt(int_t);

    void putUInt(uint_t);

    void putVarInt(varint_t);

    void putLong(long_t);

    void putULong(ulong_t);

    void putFloat(float_t);

    void putDouble(double_t);

    void putString(string_t);

    void clear();

    void reserve(size_t);

    ubyte_t *getArray();

    size_t getLimit();

    size_t getMark();

    size_t getPosition();

    size_t getSize();

    void setLimit(size_t);

    void setMark(size_t);

    void setPosition(size_t);

    void setSize(size_t);

private:
    ubytes_t buffer;
    size_t position, limit, mark;
};

#endif /* defined(__Proxy__PacketBuffer__) */
