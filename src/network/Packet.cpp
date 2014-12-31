#include "Packet.h"

Packet::Packet(varint_t packetId) : packetId(packetId) {}

Packet::~Packet() {}

varint_t Packet::getPacketId() {
    return packetId;
}

void Packet::setPacketLength(varint_t packetLength) {
    this->packetLength = packetLength;
}

void Packet::read(PacketBuffer&) {}

void Packet::write(PacketBuffer&) {}

void Packet::handle(PacketHandler*) {}