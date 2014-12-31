#include "PacketDisconnect.h"

PacketDisconnect::PacketDisconnect(PlayerConnection::Phase phase) : Packet(phase == PlayerConnection::Phase::PLAY ? 0x40 : 0x00) {}

void PacketDisconnect::write(PacketBuffer &buffer) {
    buffer.putString(reason);
}
