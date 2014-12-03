#ifndef __Proxy__PacketDisconnect__
#define __Proxy__PacketDisconnect__

#include "ServerPacket.h"

class PacketDisconnect : public ServerPacket {
public:
    string_t reason;

    PacketDisconnect();

    void write(ByteBuffer&);
};

#endif /* defined(__Proxy__PacketDisconnect__) */
