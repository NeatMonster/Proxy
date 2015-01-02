#include "PacketPluginMessage.h"

#include "PacketHandler.h"

PacketPluginMessage::PacketPluginMessage() : Packet(0x17, 0x3f) {}

void PacketPluginMessage::read(PacketBuffer &buffer) {
    size_t start = buffer.getPosition();
    buffer.getString(channel);
    data = ubytes_t(packetLength - 1 + start - buffer.getPosition());
    buffer.getUBytes(data);
}

void PacketPluginMessage::write(PacketBuffer &buffer) {
    buffer.putString(channel);
    buffer.putUBytes(data);
}

void PacketPluginMessage::handle(PacketHandler *handler) {
    handler->handlePluginMessage(this);
}
