#include "PacketEncryptionResponse.h"
#include "PacketHandler.h"

PacketEncryptionResponse::PacketEncryptionResponse() : ClientPacket(0x01) {}

void PacketEncryptionResponse::read(ByteBuffer &buffer) {
    varint_t secretLength;
    buffer.getVarInt(secretLength);
    sharedSecret = ubytes_t(secretLength);
    buffer.getUBytes(sharedSecret);
    varint_t tokenLength;
    buffer.getVarInt(tokenLength);
    verifyToken = ubytes_t(tokenLength);
    buffer.getUBytes(verifyToken);
}

void PacketEncryptionResponse::handle(PacketHandler *handler) {
    handler->handleEncryptionResponse(this);
}
