#ifndef __Proxy__PacketJoinGame__
#define __Proxy__PacketJoinGame__

#include "Packet.h"

class PacketJoinGame : public Packet {
public:
    int_t entityId;
    ubyte_t gamemode;
    byte_t dimension;
    ubyte_t difficulty;
    ubyte_t maxPlayers;
    string_t levelType;
    bool reducedDebugInfo;

    PacketJoinGame();

    void read(PacketBuffer&);

    void write(PacketBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketJoinGame__) */
