#ifndef __Proxy__ClientPacket__
#define __Proxy__ClientPacket__

#include "ByteBuffer.h"
#include "Packet.h"
#include "PacketHandler.h"

class ClientPacket : public Packet {
public:
    ClientPacket(varint_t);

    virtual ~ClientPacket();

    virtual void read(ByteBuffer&) = 0;

    virtual void handle(PacketHandler*) = 0;
};

#endif /* defined(__Proxy__ClientPacket__) */
