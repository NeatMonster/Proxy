#include "PacketSpawnPlayer.h"

#include "PacketHandler.h"

#include <iomanip>
#include <sstream>

PacketSpawnPlayer::PacketSpawnPlayer() : Packet(0x0c) {}

void PacketSpawnPlayer::read(PacketBuffer &buffer) {
    size_t start = buffer.getPosition();
    buffer.getVarInt(entityId);
    long_t msb, lsb;
    buffer.getLong(msb);
    buffer.getLong(lsb);
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << msb;
    ss << std::hex << std::setfill('0') << std::setw(16) << lsb;
    uuid = ss.str();
    uuid.insert(uuid.begin() + 8, '-');
    uuid.insert(uuid.begin() + 13, '-');
    uuid.insert(uuid.begin() + 18, '-');
    uuid.insert(uuid.begin() + 23, '-');
    buffer.getInt(x);
    buffer.getInt(y);
    buffer.getInt(z);
    buffer.getByte(yaw);
    buffer.getByte(pitch);
    buffer.getShort(currentItem);
    metadata = ubytes_t(packetLength - 1 - buffer.getPosition() + start);
    buffer.getUBytes(metadata);
}

void PacketSpawnPlayer::write(PacketBuffer &buffer) {
    buffer.putVarInt(entityId);
    buffer.putLong(std::stoull(uuid.substr(0, 8)
                               + uuid.substr(9, 4)
                               + uuid.substr(14, 4), nullptr, 16));
    buffer.putLong(std::stoull(uuid.substr(19, 4)
                               + uuid.substr(24, 12), nullptr, 16));
    buffer.putInt(x);
    buffer.putInt(y);
    buffer.putInt(z);
    buffer.putByte(yaw);
    buffer.putByte(pitch);
    buffer.putShort(currentItem);
    buffer.putUBytes(metadata);
}

void PacketSpawnPlayer::handle(PacketHandler *handler) {
    handler->handleSpawnPlayer(this);
}
