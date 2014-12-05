#include "PacketLoginSuccess.h"

PacketLoginSuccess::PacketLoginSuccess() : Packet(0x02) {}

void PacketLoginSuccess::read(ByteBuffer &buffer) {
    buffer.getString(uuid);
    buffer.getString(username);
}

void PacketLoginSuccess::write(ByteBuffer &buffer) {
    buffer.putString(uuid);
    buffer.putString(username);
}

void PacketLoginSuccess::handle(PacketHandler *handler) {
    handler->handleLoginSuccess(this);
}
