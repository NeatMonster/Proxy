#include "PacketLoginSuccess.h"

PacketLoginSuccess::PacketLoginSuccess() : Packet(0x02) {}

void PacketLoginSuccess::read(PacketBuffer &buffer) {
    buffer.getString(uuid);
    buffer.getString(username);
}

void PacketLoginSuccess::write(PacketBuffer &buffer) {
    buffer.putString(uuid);
    buffer.putString(username);
}

void PacketLoginSuccess::handle(PacketHandler *handler) {
    handler->handleLoginSuccess(this);
}
