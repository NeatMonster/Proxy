#include "Packet.h"

Packet::Packet(varint_t packetId) : packetId(packetId) {}

Packet::~Packet() {}

varint_t Packet::getPacketId() {
    return packetId;
}

void Packet::read(ByteBuffer&) {}

void Packet::write(ByteBuffer&) {}

void Packet::handle(PacketHandler*) {}
