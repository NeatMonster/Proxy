#include "PacketLoginStart.h"

#include "PacketHandler.h"

PacketLoginStart::PacketLoginStart() : Packet(0x00) {}

void PacketLoginStart::read(PacketBuffer &buffer) {
    buffer.getString(name);
}

void PacketLoginStart::write(PacketBuffer &buffer) {
    buffer.putString(name);
}

void PacketLoginStart::handle(PacketHandler *handler) {
    handler->handleLoginStart(this);
}
