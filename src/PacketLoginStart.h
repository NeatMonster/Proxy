#ifndef __Proxy__PacketLoginStart__
#define __Proxy__PacketLoginStart__

#include "ClientPacket.h"

class PacketLoginStart : public ClientPacket {
public:
    string_t name;

    PacketLoginStart();

    void read(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketLoginStart__) */
