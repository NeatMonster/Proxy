#ifndef __Proxy__PacketSpawnPlayer__
#define __Proxy__PacketSpawnPlayer__

#include "Packet.h"
#include "PacketHandler.h"

class PacketSpawnPlayer : public Packet {
public:
    varint_t entityId;
    string_t uuid;
    int_t x;
    int_t y;
    int_t z;
    byte_t yaw;
    byte_t pitch;
    short_t currentItem;
    ubytes_t metadata;

    PacketSpawnPlayer();

    void read(ByteBuffer&);

    void write(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketSpawnPlayer__) */
