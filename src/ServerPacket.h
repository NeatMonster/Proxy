#ifndef __Proxy__ServerPacket__
#define __Proxy__ServerPacket__

#include "ByteBuffer.h"
#include "Packet.h"

class ServerPacket : public Packet {
public:
    ServerPacket(varint_t packetId);

    virtual ~ServerPacket();

    virtual void write(ByteBuffer&) = 0;
};

#endif /* defined(__Proxy__ServerPacket__) */
