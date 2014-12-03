#ifndef __Proxy__PacketResponse__
#define __Proxy__PacketResponse__

#include "ServerPacket.h"

class PacketResponse : public ServerPacket {
public:
    string_t response;

    PacketResponse();

    void write(ByteBuffer&);
};

#endif /* defined(__Proxy__PacketResponse__) */
