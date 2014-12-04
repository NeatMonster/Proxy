#ifndef __Proxy__PacketSetCompression__
#define __Proxy__PacketSetCompression__

#include "ServerPacket.h"

class PacketSetCompression : public ServerPacket {
public:
    varint_t threshold;

    PacketSetCompression();

    void write(ByteBuffer&);
};

#endif /* defined(__Proxy__PacketSetCompression__) */
