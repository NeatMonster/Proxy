#ifndef __Proxy__Packet__
#define __Proxy__Packet__

#include "ByteBuffer.h"
#include "Types.h"

class PacketHandler;

class Packet {
public:
    Packet(uint_t);

    virtual ~Packet();

    varint_t getPacketId();

    void setPacketLength(varint_t);

    virtual void read(ByteBuffer&);

    virtual void write(ByteBuffer&);

    virtual void handle(PacketHandler*);

protected:
    varint_t packetId;
    varint_t packetLength;
};

#endif /* defined(__Proxy__Packet__) */
