#ifndef __Proxy__PacketHandshake__
#define __Proxy__PacketHandshake__

#include "Packet.h"

class PacketHandshake : public Packet {
public:
    varint_t protocolVersion;
    string_t serverAddress;
    ushort_t serverPort;
    varint_t nextState;

    PacketHandshake();

    void read(ByteBuffer&);

    void write(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketHandshake__) */
