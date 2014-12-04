#ifndef __Proxy__PacketEncryptionResponse__
#define __Proxy__PacketEncryptionResponse__

#include "ClientPacket.h"

class PacketEncryptionResponse : public ClientPacket {
public:
    ubytes_t sharedSecret;
    ubytes_t verifyToken;

    PacketEncryptionResponse();

    void read(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketEncryptionResponse__) */
