#include "PacketSetCompression.h"

PacketSetCompression::PacketSetCompression() : Packet(0x03) {}

void PacketSetCompression::write(PacketBuffer &buffer) {
    buffer.putVarInt(threshold);
}
