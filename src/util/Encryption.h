#ifndef __Proxy__Encryption__
#define __Proxy__Encryption__

#include "Types.h"

#include "polarssl/ctr_drbg.h"
#include "polarssl/rsa.h"

class Encryption {
public:
    static void initialize();

    static void decrypt(ubyte_t*, ubyte_t*, size_t*);

    static ubytes_t getPublicKey();

    static ctr_drbg_context *getRandom();

private:
    static rsa_context rsa;
    static ctr_drbg_context ctr_drbg;
    static ubytes_t publicKey;
};

#endif /* defined(__Proxy__Encryption__) */
