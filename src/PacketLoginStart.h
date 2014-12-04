#ifndef __Proxy__PacketLoginStart__
#define __Proxy__PacketLoginStart__

#include "Packet.h"

class PacketLoginStart : public Packet {
public:
    string_t name;

    PacketLoginStart();

    void read(ByteBuffer&);

    void write(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketLoginStart__) */
