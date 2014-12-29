#include "PacketDisconnect.h"

PacketDisconnect::PacketDisconnect() : Packet(0x00) {}

void PacketDisconnect::write(PacketBuffer &buffer) {
    buffer.putString(reason);
}
