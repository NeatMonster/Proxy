#include "PacketRequest.h"

PacketRequest::PacketRequest() : ClientPacket(0x00) {}

void PacketRequest::read(ByteBuffer&) {}

void PacketRequest::handle(PacketHandler *handler) {
    handler->handleRequest(this);
}
