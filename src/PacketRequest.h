#ifndef __Proxy__PacketRequest__
#define __Proxy__PacketRequest__

#include "Packet.h"

class PacketRequest : public Packet {
public:
    PacketRequest();

    void read(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketRequest__) */
