#include "Encryption.h"

#include "Logger.h"

#include "polarssl/x509.h"
#include "polarssl/entropy.h"

#include <cstring>

void Encryption::initialize() {
    Logger::info() << "Génération de la paire de clefs" << std::endl;
    entropy_context entropy;
    entropy_init(&entropy);
    ctr_drbg_init(&ctr_drbg, entropy_func, &entropy, (ubyte_t*) "nbuijmef", 8);
    rsa_init(&rsa, RSA_PKCS_V15, 0);
    rsa_gen_key(&rsa, ctr_drbg_random, &ctr_drbg, 1024, 65537);
    pk_context pk;
    pk_init(&pk);
    pk_init_ctx(&pk, pk_info_from_type(POLARSSL_PK_RSA));
    rsa_copy(pk_rsa(pk), &rsa);
    ubyte_t buffer[1024];
    int length = pk_write_pubkey_der(&pk, buffer, sizeof(buffer));
    publicKey = ubytes_t(length);
    std::memcpy(publicKey.data(), &buffer[1024 - length], length);
    entropy_free(&entropy);
    pk_free(&pk);
}

void Encryption::decrypt(ubyte_t *cipher, ubyte_t *plain, size_t *length) {
    rsa_pkcs1_decrypt(&rsa, ctr_drbg_random, &ctr_drbg, RSA_PRIVATE, length, cipher, plain, *length);
}

ubytes_t Encryption::getPublicKey() {
    return publicKey;
}

ctr_drbg_context *Encryption::getRandom() {
    return &ctr_drbg;
}

rsa_context Encryption::rsa;
ctr_drbg_context Encryption::ctr_drbg;
ubytes_t Encryption::publicKey;