#include "PacketEncryptionRequest.h"

PacketEncryptionRequest::PacketEncryptionRequest() : Packet(0x01) {}

void PacketEncryptionRequest::write(PacketBuffer &buffer) {
    buffer.putString(serverId);
    buffer.putVarInt(publicKey.size());
    buffer.putUBytes(publicKey);
    buffer.putVarInt(verifyToken.size());
    buffer.putUBytes(verifyToken);
}
