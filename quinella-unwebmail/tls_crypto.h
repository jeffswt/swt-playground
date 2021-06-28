
#ifndef _QUINELLA_TLS_CRYPTO_H
#define _QUINELLA_TLS_CRYPTO_H

#include <iostream>
#include <openssl/hmac.h>
#include "tls_cipher_suite.h"


/// Calculate HMAC values.
/// @param proto: HMAC protocol to be used, e.g. SHA256.
/// @param secret: Secret string, in binary
/// @param seed: Seed string, in binary
/// @returns HMAC_proto(secret, seed)
std::string tls_key_hmachash(CipherHmacProtocol proto, std::string secret,
    std::string seed);

/// Calculate Phash values.
/// @param proto: HMAC protocol to be used, e.g. SHA256.
/// @param secret: Secret string, in binary
/// @param seed: Seed string, in binary
/// @returns P_proto(secret, seed)
std::string tls_key_phash(CipherHmacProtocol proto, std::string secret,
    std::string seed, int length);

/// Calculate pseudo-random values.
/// @param proto: HMAC protocol to be used, e.g. SHA256.
/// @param secret: Secret string, in binary
/// @param label: Label string, in binary
/// @param seed: Seed string, in binary
/// @returns PRF_proto(secret, label, seed)
std::string tls_key_prf(CipherHmacProtocol proto, std::string secret,
    std::string label, std::string seed, int length);

#endif  // _QUINELLA_TLS_CRYPTO_H
