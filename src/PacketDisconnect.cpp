#include "PacketDisconnect.h"

PacketDisconnect::PacketDisconnect() : Packet(0x00) {}

void PacketDisconnect::write(ByteBuffer &buffer) {
    buffer.putString(reason);
}
