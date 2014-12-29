#ifndef __Proxy__PacketResponse__
#define __Proxy__PacketResponse__

#include "Packet.h"

class PacketResponse : public Packet {
public:
    string_t response;

    PacketResponse();

    void write(PacketBuffer&);
};

#endif /* defined(__Proxy__PacketResponse__) */
