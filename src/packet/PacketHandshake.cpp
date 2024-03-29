#include "PacketHandshake.h"

#include "PacketHandler.h"

PacketHandshake::PacketHandshake() : Packet(0x00) {}

void PacketHandshake::read(PacketBuffer &buffer) {
    buffer.getVarInt(protocolVersion);
    buffer.getString(serverAddress);
    buffer.getUShort(serverPort);
    buffer.getVarInt(nextState);
}

void PacketHandshake::write(PacketBuffer &buffer) {
    buffer.putVarInt(protocolVersion);
    buffer.putString(serverAddress);
    buffer.putUShort(serverPort);
    buffer.putVarInt(nextState);
}

void PacketHandshake::handle(PacketHandler *handler) {
    handler->handleHandshake(this);
}
