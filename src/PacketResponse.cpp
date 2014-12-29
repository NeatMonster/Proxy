#include "PacketResponse.h"

PacketResponse::PacketResponse() : Packet(0x00) {}

void PacketResponse::write(PacketBuffer &buffer) {
    buffer.putString(response);
}
