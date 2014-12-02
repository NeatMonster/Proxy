#include "ClientPacket.h"

ClientPacket::ClientPacket(varint_t packetId) : Packet(packetId) {}

ClientPacket::~ClientPacket() {}
