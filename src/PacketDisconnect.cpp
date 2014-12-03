#include "PacketDisconnect.h"

PacketDisconnect::PacketDisconnect() : ServerPacket(0x00) {}

void PacketDisconnect::write(ByteBuffer &buffer) {
    buffer.putString(reason);
}
