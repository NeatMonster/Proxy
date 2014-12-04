#ifndef __Proxy__PacketPing__
#define __Proxy__PacketPing__

#include "Packet.h"

class PacketPing : public Packet {
public:
    long_t time;

    PacketPing();

    void read(ByteBuffer&);

    void write(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketPing__) */
