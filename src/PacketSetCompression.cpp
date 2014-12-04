#include "PacketSetCompression.h"

PacketSetCompression::PacketSetCompression() : ServerPacket(0x03) {}

void PacketSetCompression::write(ByteBuffer &buffer) {
    buffer.putVarInt(threshold);
}
