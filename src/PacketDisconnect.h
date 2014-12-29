#ifndef __Proxy__PacketDisconnect__
#define __Proxy__PacketDisconnect__

#include "Packet.h"

class PacketDisconnect : public Packet {
public:
    string_t reason;

    PacketDisconnect();

    void write(PacketBuffer&);
};

#endif /* defined(__Proxy__PacketDisconnect__) */
