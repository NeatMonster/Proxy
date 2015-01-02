#include "Packet.h"

Packet::Packet(varint_t packetId) : Packet::Packet(packetId, packetId) {}

Packet::Packet(varint_t clientPacketId, varint_t serverPacketId) :
    clientPacketId(clientPacketId), serverPacketId(serverPacketId) {}

Packet::~Packet() {}

varint_t Packet::getClientPacketId() {
    return clientPacketId;
}

varint_t Packet::getServerPacketId() {
    return serverPacketId;
}

void Packet::setPacketLength(varint_t packetLength) {
    this->packetLength = packetLength;
}

void Packet::read(PacketBuffer&) {}

void Packet::write(PacketBuffer&) {}

void Packet::handle(PacketHandler*) {}
