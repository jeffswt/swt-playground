
#ifndef _QUINELLA_TLS_CIPHER_SUITE_H
#define _QUINELLA_TLS_CIPHER_SUITE_H

#include <iostream>
#include <sys/types.h>


/// Cipher suite keypair negotiation protocol..
enum CipherNegotiationProtocol {
    TLS_CNP_NONE,
    TLS_CNP_DHE_DSS,
    TLS_CNP_DHE_DSS_EXPORT,
    TLS_CNP_DHE_PSK,
    TLS_CNP_DHE_RSA,
    TLS_CNP_DHE_RSA_EXPORT,
    TLS_CNP_DH_DSS,
    TLS_CNP_DH_DSS_EXPORT,
    TLS_CNP_DH_RSA,
    TLS_CNP_DH_RSA_EXPORT,
    TLS_CNP_DH_anon,
    TLS_CNP_DH_anon_EXPORT,
    TLS_CNP_ECCPWD,
    TLS_CNP_ECDHE_ECDSA,
    TLS_CNP_ECDHE_PSK,
    TLS_CNP_ECDHE_RSA,
    TLS_CNP_ECDH_ECDSA,
    TLS_CNP_ECDH_RSA,
    TLS_CNP_ECDH_anon,
    TLS_CNP_GOSTR341112_256,
    TLS_CNP_KRB5,
    TLS_CNP_KRB5_EXPORT,
    TLS_CNP_PSK,
    TLS_CNP_PSK_DHE,
    TLS_CNP_RSA,
    TLS_CNP_RSA_EXPORT,
    TLS_CNP_RSA_PSK,
    TLS_CNP_SRP_SHA,
    TLS_CNP_SRP_SHA_DSS,
    TLS_CNP_SRP_SHA_RSA,
    TLS_CNP_UNKNOWN,
};

/// Cipher suite symmetric encryption protocol.
enum CipherEncryptionProtocol {
    TLS_CEP_NONE,
    TLS_CEP__28147_CNT,
    TLS_CEP__3DES_EDE_CBC,
    TLS_CEP_AES_128_CBC,
    TLS_CEP_AES_128_CCM,
    TLS_CEP_AES_128_CCM_8,
    TLS_CEP_AES_128_GCM,
    TLS_CEP_AES_256_CBC,
    TLS_CEP_AES_256_CCM,
    TLS_CEP_AES_256_CCM_8,
    TLS_CEP_AES_256_GCM,
    TLS_CEP_ARIA_128_CBC,
    TLS_CEP_ARIA_128_GCM,
    TLS_CEP_ARIA_256_CBC,
    TLS_CEP_ARIA_256_GCM,
    TLS_CEP_CAMELLIA_128_CBC,
    TLS_CEP_CAMELLIA_128_GCM,
    TLS_CEP_CAMELLIA_256_CBC,
    TLS_CEP_CAMELLIA_256_GCM,
    TLS_CEP_CHACHA20,
    TLS_CEP_CHACHA20_POLY1305,
    TLS_CEP_DES40_CBC,
    TLS_CEP_DES_CBC,
    TLS_CEP_DES_CBC_40,
    TLS_CEP_IDEA_CBC,
    TLS_CEP_KUZNYECHIK,
    TLS_CEP_KUZNYECHIK_CTR,
    TLS_CEP_KUZNYECHIK_MGM,
    TLS_CEP_MAGMA,
    TLS_CEP_MAGMA_CTR,
    TLS_CEP_RC2_CBC_40,
    TLS_CEP_RC4_128,
    TLS_CEP_RC4_40,
    TLS_CEP_SEED_CBC,
    TLS_CEP_SM4_CCM,
    TLS_CEP_SM4_GCM,
    TLS_CEP_UNKNOWN,
};

/// Cipher suite authenticity verification protocol.
enum CipherHmacProtocol {
    TLS_CHP_NONE,
    TLS_CHP_IMIT,
    TLS_CHP_MD5,
    TLS_CHP_MGM_L,
    TLS_CHP_MGM_S,
    TLS_CHP_OMAC,
    TLS_CHP_SHA,
    TLS_CHP_SHA256,
    TLS_CHP_SHA384,
    TLS_CHP_SM3,
    TLS_CHP_UNKNOWN,
};

/// TLS cipher suite mappings.
/// TLS 1.2: https://datatracker.ietf.org/doc/html/rfc5246#appendix-A.5
/// ECDH: https://datatracker.ietf.org/doc/html/rfc4492#section-6
/// AES GCM: https://datatracker.ietf.org/doc/html/rfc5289#section-3
/// Full table: https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml
class CipherSuite {
public:
    uint16_t value;
    std::string name;
    CipherNegotiationProtocol neg;
    CipherEncryptionProtocol enc;
    CipherHmacProtocol hmac;
    CipherSuite(uint16_t value, std::string name,
        CipherNegotiationProtocol neg, CipherEncryptionProtocol enc,
        CipherHmacProtocol hmac);
    bool operator == (const CipherSuite &other) const;
};

/// Get cipher suite by 16-bit ID.
/// @param key: Cipher suite ID defined by IANA.
/// @return Cipher suite object.
/// @exception Throws CapException if no such cipher suite found.
CipherSuite tls_get_cipher_suite(uint16_t key);

#endif  // _QUINELLA_TLS_CIPHER_SUITE_H
