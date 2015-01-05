#include "PacketRespawn.h"

PacketRespawn::PacketRespawn() : Packet(0x07) {}

void PacketRespawn::write(PacketBuffer &buffer) {
    buffer.putInt(dimension);
    buffer.putUByte(difficulty);
    buffer.putUByte(gamemode);
    buffer.putString(levelType);
}
