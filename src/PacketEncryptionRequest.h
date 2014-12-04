#ifndef __Proxy__PacketEncryptionRequest__
#define __Proxy__PacketEncryptionRequest__

#include "Packet.h"

class PacketEncryptionRequest : public Packet {
public:
    string_t serverId;
    ubytes_t publicKey;
    ubytes_t verifyToken;

    PacketEncryptionRequest();

    void write(ByteBuffer&);
};

#endif /* defined(__Proxy__PacketEncryptionRequest__) */
