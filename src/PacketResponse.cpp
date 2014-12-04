#include "PacketResponse.h"

PacketResponse::PacketResponse() : Packet(0x00) {}

void PacketResponse::write(ByteBuffer &buffer) {
    buffer.putString(response);
}
