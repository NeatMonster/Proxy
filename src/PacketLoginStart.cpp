#include "PacketLoginStart.h"
#include "PacketHandler.h"

PacketLoginStart::PacketLoginStart() : ClientPacket(0x00) {}

void PacketLoginStart::read(ByteBuffer &buffer) {
    buffer.getString(name);
}

void PacketLoginStart::handle(PacketHandler *handler) {
    handler->handleLoginStart(this);
}
