#ifndef __Proxy__PacketEncryptionResponse__
#define __Proxy__PacketEncryptionResponse__

#include "Packet.h"

class PacketEncryptionResponse : public Packet {
public:
    ubytes_t sharedSecret;
    ubytes_t verifyToken;

    PacketEncryptionResponse();

    void read(PacketBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketEncryptionResponse__) */
