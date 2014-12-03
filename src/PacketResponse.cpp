#include "PacketResponse.h"

PacketResponse::PacketResponse() : ServerPacket(0x00) {}

void PacketResponse::write(ByteBuffer &buffer) {
    buffer.putString(response);
}
