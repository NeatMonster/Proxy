#ifndef __Proxy__PacketPluginMessage__
#define __Proxy__PacketPluginMessage__

#include "Packet.h"

class PacketPluginMessage : public Packet {
public:
    string_t channel;
    ubytes_t data;

    PacketPluginMessage();

    void read(PacketBuffer&);

    void write(PacketBuffer&);

    void handle(PacketHandler*);
};


#endif /* defined(__Proxy__PacketPluginMessage__) */
