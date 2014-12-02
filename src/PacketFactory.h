#ifndef __Proxy__PacketFactory__
#define __Proxy__PacketFactory__

#include "ClientPacket.h"

#include <map>

class PacketFactory {
public:
    enum Phase {
        HANDSHAKE, STATUS, LOGIN, PLAY
    };

    PacketFactory(Phase);

    ClientPacket *createPacket(const varint_t);

    bool hasPacket(const varint_t);

private:
    typedef ClientPacket *(*cons_t)();
    std::map<const varint_t, cons_t> packets;

    template<typename T>
    static ClientPacket *constructPacket();

    template<typename T>
    void registerPacket(const varint_t);
};

#endif /* defined(__Proxy__PacketFactory__) */
