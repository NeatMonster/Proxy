#ifndef __Proxy__PacketLoginSuccess__
#define __Proxy__PacketLoginSuccess__

#include "Packet.h"

#include "PacketHandler.h"

class PacketLoginSuccess : public Packet {
public:
    string_t uuid;
    string_t username;

    PacketLoginSuccess();

    void read(PacketBuffer&);

    void write(PacketBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketLoginSuccess__) */
