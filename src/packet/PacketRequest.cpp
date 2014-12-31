#include "PacketRequest.h"

#include "PacketHandler.h"

PacketRequest::PacketRequest() : Packet(0x00) {}

void PacketRequest::read(PacketBuffer&) {}

void PacketRequest::handle(PacketHandler *handler) {
    handler->handleRequest(this);
}
