#ifndef __Proxy__PacketSetCompression__
#define __Proxy__PacketSetCompression__

#include "Packet.h"

class PacketSetCompression : public Packet {
public:
    varint_t threshold;

    PacketSetCompression();

    void write(PacketBuffer&);
};

#endif /* defined(__Proxy__PacketSetCompression__) */
