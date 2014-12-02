#include "PacketHandshake.h"

PacketHandshake::PacketHandshake() : ClientPacket(0x00) {}

void PacketHandshake::read(ByteBuffer &buffer) {
    buffer.getVarInt(protocolVersion);
    buffer.getString(serverAddress);
    buffer.getUShort(serverPort);
    buffer.getVarInt(nextState);
}
