#include "PacketJoinGame.h"

#include "PacketHandler.h"

PacketJoinGame::PacketJoinGame() : Packet(0x01) {}

void PacketJoinGame::read(PacketBuffer &buffer) {
    buffer.getInt(entityId);
    buffer.getUByte(gamemode);
    buffer.getByte(dimension);
    buffer.getUByte(difficulty);
    buffer.getUByte(maxPlayers);
    buffer.getString(levelType);
    buffer.getBool(reducedDebugInfo);
}

void PacketJoinGame::write(PacketBuffer &buffer) {
    buffer.putInt(entityId);
    buffer.putUByte(gamemode);
    buffer.putByte(dimension);
    buffer.putUByte(difficulty);
    buffer.putUByte(maxPlayers);
    buffer.putString(levelType);
    buffer.putBool(reducedDebugInfo);
}

void PacketJoinGame::handle(PacketHandler *handler) {
    handler->handleJoinGame(this);
}
