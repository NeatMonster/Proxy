#include "PacketLoginStart.h"

#include "PacketHandler.h"

PacketLoginStart::PacketLoginStart() : Packet(0x00) {}

void PacketLoginStart::read(ByteBuffer &buffer) {
    buffer.getString(name);
}

void PacketLoginStart::write(ByteBuffer &buffer) {
    buffer.putString(name);
}

void PacketLoginStart::handle(PacketHandler *handler) {
    handler->handleLoginStart(this);
}
