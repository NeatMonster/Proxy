#include "PacketSetCompression.h"

PacketSetCompression::PacketSetCompression() : Packet(0x03) {}

void PacketSetCompression::write(ByteBuffer &buffer) {
    buffer.putVarInt(threshold);
}
