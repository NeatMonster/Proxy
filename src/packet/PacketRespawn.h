#ifndef __Proxy__PacketRespawn__
#define __Proxy__PacketRespawn__

#include "Packet.h"

class PacketRespawn : public Packet {
public:
    int_t dimension;
    ubyte_t difficulty;
    ubyte_t gamemode;
    string_t levelType;

    PacketRespawn();

    void write(PacketBuffer&);
};

#endif /* defined(__Proxy__PacketRespawn__) */
