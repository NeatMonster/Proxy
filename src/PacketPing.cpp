#include "PacketPing.h"

#include "PacketHandler.h"

PacketPing::PacketPing() : Packet(0x01) {}

void PacketPing::read(ByteBuffer &buffer) {
    buffer.getLong(time);
}

void PacketPing::write(ByteBuffer &buffer) {
    buffer.putLong(time);
}

void PacketPing::handle(PacketHandler *handler) {
    handler->handlePing(this);
}
