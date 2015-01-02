#ifndef __Proxy__Packet__
#define __Proxy__Packet__

#include "PacketBuffer.h"
#include "Types.h"

class PacketHandler;

class Packet {
public:
    Packet(varint_t);

    Packet(varint_t, varint_t);

    virtual ~Packet();

    varint_t getClientPacketId();

    varint_t getServerPacketId();

    void setPacketLength(varint_t);

    virtual void read(PacketBuffer&);

    virtual void write(PacketBuffer&);

    virtual void handle(PacketHandler*);

protected:
    varint_t clientPacketId;
    varint_t serverPacketId;
    varint_t packetLength;
};

#endif /* defined(__Proxy__Packet__) */
