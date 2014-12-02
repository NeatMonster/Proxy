#ifndef __Proxy__Packet__
#define __Proxy__Packet__

#include "Types.h"

class Packet {
public:
    Packet(uint_t);

    virtual ~Packet();

    varint_t getPacketId();

private:
    varint_t packetId;
};

#endif /* defined(__Proxy__Packet__) */
