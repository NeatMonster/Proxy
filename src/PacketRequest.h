#ifndef __Proxy__PacketRequest__
#define __Proxy__PacketRequest__

#include "ClientPacket.h"

class PacketRequest : public ClientPacket {
public:
    PacketRequest();

    void read(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketRequest__) */
