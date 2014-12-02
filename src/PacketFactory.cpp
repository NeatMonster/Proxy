#include "PacketFactory.h"

#include "PacketHandshake.h"

PacketFactory::PacketFactory(Phase phase) {
    switch (phase) {
        case HANDSHAKE:
            registerPacket<PacketHandshake>(0x00);
            break;
        default:
            break;
    }
}

ClientPacket *PacketFactory::createPacket(const varint_t packetId) {
    return packets.find(packetId)->second();
}

bool PacketFactory::hasPacket(const varint_t packetId) {
    return packets.find(packetId) != packets.end();
}

template<typename T>
ClientPacket *PacketFactory::constructPacket() {
    return new T;
}

template<typename T>
void PacketFactory::registerPacket(const varint_t packetId) {
    packets.insert(std::make_pair(packetId, &PacketFactory::constructPacket<T>));
}
