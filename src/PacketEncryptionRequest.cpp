#include "PacketEncryptionRequest.h"

PacketEncryptionRequest::PacketEncryptionRequest() : ServerPacket(0x01) {}

void PacketEncryptionRequest::write(ByteBuffer &buffer) {
    buffer.putString(serverId);
    buffer.putVarInt(publicKey.size());
    buffer.putUBytes(publicKey);
    buffer.putVarInt(verifyToken.size());
    buffer.putUBytes(verifyToken);
}
