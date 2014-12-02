#include "Packet.h"

Packet::Packet(varint_t packetId) : packetId(packetId) {}

Packet::~Packet() {}

varint_t Packet::getPacketId() {
    return packetId;
}
