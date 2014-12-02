#ifndef __Proxy__PacketHandshake__
#define __Proxy__PacketHandshake__

#include "ClientPacket.h"

class PacketHandshake : public ClientPacket {
public:
    varint_t protocolVersion;
    string_t serverAddress;
    ushort_t serverPort;
    varint_t nextState;

    PacketHandshake();

    void read(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketHandshake__) */
