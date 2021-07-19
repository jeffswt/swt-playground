
#include "tls_cipher_suite.h"

#include "utils.h"
#include <vector>


CipherSuite::CipherSuite(uint16_t value, std::string name,
    CipherNegotiationProtocol neg, CipherEncryptionProtocol enc,
    CipherHmacProtocol hmac) : value(value), name(name), neg(neg), enc(enc),
    hmac(hmac) {}

bool CipherSuite::operator == (const CipherSuite &other) const {
    return this->value == other.value;
}

CipherSuite tls_get_cipher_suite(uint16_t key) {
    #define cs(a,b,c,d,e)  CipherSuite(b, "#a",                               \
        CipherNegotiationProtocol::TLS_CNP_##c,                               \
        CipherEncryptionProtocol::TLS_CEP_##d,                                \
        CipherHmacProtocol::TLS_CHP_##e)
    static std::vector<CipherSuite> cipher_suites = {
        cs(TLS_NULL_WITH_NULL_NULL, 0x0000, NONE, NONE, NONE),
        cs(TLS_RSA_WITH_NULL_MD5, 0x0001, RSA, NONE, MD5),
        cs(TLS_RSA_WITH_NULL_SHA, 0x0002, RSA, NONE, SHA),
        cs(TLS_RSA_EXPORT_WITH_RC4_40_MD5, 0x0003, RSA_EXPORT, RC4_40, MD5),
        cs(TLS_RSA_WITH_RC4_128_MD5, 0x0004, RSA, RC4_128, MD5),
        cs(TLS_RSA_WITH_RC4_128_SHA, 0x0005, RSA, RC4_128, SHA),
        cs(TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5, 0x0006, RSA_EXPORT, RC2_CBC_40, MD5),
        cs(TLS_RSA_WITH_IDEA_CBC_SHA, 0x0007, RSA, IDEA_CBC, SHA),
        cs(TLS_RSA_EXPORT_WITH_DES40_CBC_SHA, 0x0008, RSA_EXPORT, DES40_CBC, SHA),
        cs(TLS_RSA_WITH_DES_CBC_SHA, 0x0009, RSA, DES_CBC, SHA),
        cs(TLS_RSA_WITH_3DES_EDE_CBC_SHA, 0x000A, RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA, 0x000B, DH_DSS_EXPORT, DES40_CBC, SHA),
        cs(TLS_DH_DSS_WITH_DES_CBC_SHA, 0x000C, DH_DSS, DES_CBC, SHA),
        cs(TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA, 0x000D, DH_DSS, _3DES_EDE_CBC, SHA),
        cs(TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA, 0x000E, DH_RSA_EXPORT, DES40_CBC, SHA),
        cs(TLS_DH_RSA_WITH_DES_CBC_SHA, 0x000F, DH_RSA, DES_CBC, SHA),
        cs(TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA, 0x0010, DH_RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA, 0x0011, DHE_DSS_EXPORT, DES40_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_DES_CBC_SHA, 0x0012, DHE_DSS, DES_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA, 0x0013, DHE_DSS, _3DES_EDE_CBC, SHA),
        cs(TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA, 0x0014, DHE_RSA_EXPORT, DES40_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_DES_CBC_SHA, 0x0015, DHE_RSA, DES_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA, 0x0016, DHE_RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_DH_anon_EXPORT_WITH_RC4_40_MD5, 0x0017, DH_anon_EXPORT, RC4_40, MD5),
        cs(TLS_DH_anon_WITH_RC4_128_MD5, 0x0018, DH_anon, RC4_128, MD5),
        cs(TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA, 0x0019, DH_anon_EXPORT, DES40_CBC, SHA),
        cs(TLS_DH_anon_WITH_DES_CBC_SHA, 0x001A, DH_anon, DES_CBC, SHA),
        cs(TLS_DH_anon_WITH_3DES_EDE_CBC_SHA, 0x001B, DH_anon, _3DES_EDE_CBC, SHA),
        cs(TLS_KRB5_WITH_DES_CBC_SHA, 0x001E, KRB5, DES_CBC, SHA),
        cs(TLS_KRB5_WITH_3DES_EDE_CBC_SHA, 0x001F, KRB5, _3DES_EDE_CBC, SHA),
        cs(TLS_KRB5_WITH_RC4_128_SHA, 0x0020, KRB5, RC4_128, SHA),
        cs(TLS_KRB5_WITH_IDEA_CBC_SHA, 0x0021, KRB5, IDEA_CBC, SHA),
        cs(TLS_KRB5_WITH_DES_CBC_MD5, 0x0022, KRB5, DES_CBC, MD5),
        cs(TLS_KRB5_WITH_3DES_EDE_CBC_MD5, 0x0023, KRB5, _3DES_EDE_CBC, MD5),
        cs(TLS_KRB5_WITH_RC4_128_MD5, 0x0024, KRB5, RC4_128, MD5),
        cs(TLS_KRB5_WITH_IDEA_CBC_MD5, 0x0025, KRB5, IDEA_CBC, MD5),
        cs(TLS_KRB5_EXPORT_WITH_DES_CBC_40_SHA, 0x0026, KRB5_EXPORT, DES_CBC_40, SHA),
        cs(TLS_KRB5_EXPORT_WITH_RC2_CBC_40_SHA, 0x0027, KRB5_EXPORT, RC2_CBC_40, SHA),
        cs(TLS_KRB5_EXPORT_WITH_RC4_40_SHA, 0x0028, KRB5_EXPORT, RC4_40, SHA),
        cs(TLS_KRB5_EXPORT_WITH_DES_CBC_40_MD5, 0x0029, KRB5_EXPORT, DES_CBC_40, MD5),
        cs(TLS_KRB5_EXPORT_WITH_RC2_CBC_40_MD5, 0x002A, KRB5_EXPORT, RC2_CBC_40, MD5),
        cs(TLS_KRB5_EXPORT_WITH_RC4_40_MD5, 0x002B, KRB5_EXPORT, RC4_40, MD5),
        cs(TLS_PSK_WITH_NULL_SHA, 0x002C, PSK, NONE, SHA),
        cs(TLS_DHE_PSK_WITH_NULL_SHA, 0x002D, DHE_PSK, NONE, SHA),
        cs(TLS_RSA_PSK_WITH_NULL_SHA, 0x002E, RSA_PSK, NONE, SHA),
        cs(TLS_RSA_WITH_AES_128_CBC_SHA, 0x002F, RSA, AES_128_CBC, SHA),
        cs(TLS_DH_DSS_WITH_AES_128_CBC_SHA, 0x0030, DH_DSS, AES_128_CBC, SHA),
        cs(TLS_DH_RSA_WITH_AES_128_CBC_SHA, 0x0031, DH_RSA, AES_128_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_AES_128_CBC_SHA, 0x0032, DHE_DSS, AES_128_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_AES_128_CBC_SHA, 0x0033, DHE_RSA, AES_128_CBC, SHA),
        cs(TLS_DH_anon_WITH_AES_128_CBC_SHA, 0x0034, DH_anon, AES_128_CBC, SHA),
        cs(TLS_RSA_WITH_AES_256_CBC_SHA, 0x0035, RSA, AES_256_CBC, SHA),
        cs(TLS_DH_DSS_WITH_AES_256_CBC_SHA, 0x0036, DH_DSS, AES_256_CBC, SHA),
        cs(TLS_DH_RSA_WITH_AES_256_CBC_SHA, 0x0037, DH_RSA, AES_256_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_AES_256_CBC_SHA, 0x0038, DHE_DSS, AES_256_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_AES_256_CBC_SHA, 0x0039, DHE_RSA, AES_256_CBC, SHA),
        cs(TLS_DH_anon_WITH_AES_256_CBC_SHA, 0x003A, DH_anon, AES_256_CBC, SHA),
        cs(TLS_RSA_WITH_NULL_SHA256, 0x003B, RSA, NONE, SHA256),
        cs(TLS_RSA_WITH_AES_128_CBC_SHA256, 0x003C, RSA, AES_128_CBC, SHA256),
        cs(TLS_RSA_WITH_AES_256_CBC_SHA256, 0x003D, RSA, AES_256_CBC, SHA256),
        cs(TLS_DH_DSS_WITH_AES_128_CBC_SHA256, 0x003E, DH_DSS, AES_128_CBC, SHA256),
        cs(TLS_DH_RSA_WITH_AES_128_CBC_SHA256, 0x003F, DH_RSA, AES_128_CBC, SHA256),
        cs(TLS_DHE_DSS_WITH_AES_128_CBC_SHA256, 0x0040, DHE_DSS, AES_128_CBC, SHA256),
        cs(TLS_RSA_WITH_CAMELLIA_128_CBC_SHA, 0x0041, RSA, CAMELLIA_128_CBC, SHA),
        cs(TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA, 0x0042, DH_DSS, CAMELLIA_128_CBC, SHA),
        cs(TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA, 0x0043, DH_RSA, CAMELLIA_128_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA, 0x0044, DHE_DSS, CAMELLIA_128_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA, 0x0045, DHE_RSA, CAMELLIA_128_CBC, SHA),
        cs(TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA, 0x0046, DH_anon, CAMELLIA_128_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_AES_128_CBC_SHA256, 0x0067, DHE_RSA, AES_128_CBC, SHA256),
        cs(TLS_DH_DSS_WITH_AES_256_CBC_SHA256, 0x0068, DH_DSS, AES_256_CBC, SHA256),
        cs(TLS_DH_RSA_WITH_AES_256_CBC_SHA256, 0x0069, DH_RSA, AES_256_CBC, SHA256),
        cs(TLS_DHE_DSS_WITH_AES_256_CBC_SHA256, 0x006A, DHE_DSS, AES_256_CBC, SHA256),
        cs(TLS_DHE_RSA_WITH_AES_256_CBC_SHA256, 0x006B, DHE_RSA, AES_256_CBC, SHA256),
        cs(TLS_DH_anon_WITH_AES_128_CBC_SHA256, 0x006C, DH_anon, AES_128_CBC, SHA256),
        cs(TLS_DH_anon_WITH_AES_256_CBC_SHA256, 0x006D, DH_anon, AES_256_CBC, SHA256),
        cs(TLS_RSA_WITH_CAMELLIA_256_CBC_SHA, 0x0084, RSA, CAMELLIA_256_CBC, SHA),
        cs(TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA, 0x0085, DH_DSS, CAMELLIA_256_CBC, SHA),
        cs(TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA, 0x0086, DH_RSA, CAMELLIA_256_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA, 0x0087, DHE_DSS, CAMELLIA_256_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA, 0x0088, DHE_RSA, CAMELLIA_256_CBC, SHA),
        cs(TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA, 0x0089, DH_anon, CAMELLIA_256_CBC, SHA),
        cs(TLS_PSK_WITH_RC4_128_SHA, 0x008A, PSK, RC4_128, SHA),
        cs(TLS_PSK_WITH_3DES_EDE_CBC_SHA, 0x008B, PSK, _3DES_EDE_CBC, SHA),
        cs(TLS_PSK_WITH_AES_128_CBC_SHA, 0x008C, PSK, AES_128_CBC, SHA),
        cs(TLS_PSK_WITH_AES_256_CBC_SHA, 0x008D, PSK, AES_256_CBC, SHA),
        cs(TLS_DHE_PSK_WITH_RC4_128_SHA, 0x008E, DHE_PSK, RC4_128, SHA),
        cs(TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA, 0x008F, DHE_PSK, _3DES_EDE_CBC, SHA),
        cs(TLS_DHE_PSK_WITH_AES_128_CBC_SHA, 0x0090, DHE_PSK, AES_128_CBC, SHA),
        cs(TLS_DHE_PSK_WITH_AES_256_CBC_SHA, 0x0091, DHE_PSK, AES_256_CBC, SHA),
        cs(TLS_RSA_PSK_WITH_RC4_128_SHA, 0x0092, RSA_PSK, RC4_128, SHA),
        cs(TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA, 0x0093, RSA_PSK, _3DES_EDE_CBC, SHA),
        cs(TLS_RSA_PSK_WITH_AES_128_CBC_SHA, 0x0094, RSA_PSK, AES_128_CBC, SHA),
        cs(TLS_RSA_PSK_WITH_AES_256_CBC_SHA, 0x0095, RSA_PSK, AES_256_CBC, SHA),
        cs(TLS_RSA_WITH_SEED_CBC_SHA, 0x0096, RSA, SEED_CBC, SHA),
        cs(TLS_DH_DSS_WITH_SEED_CBC_SHA, 0x0097, DH_DSS, SEED_CBC, SHA),
        cs(TLS_DH_RSA_WITH_SEED_CBC_SHA, 0x0098, DH_RSA, SEED_CBC, SHA),
        cs(TLS_DHE_DSS_WITH_SEED_CBC_SHA, 0x0099, DHE_DSS, SEED_CBC, SHA),
        cs(TLS_DHE_RSA_WITH_SEED_CBC_SHA, 0x009A, DHE_RSA, SEED_CBC, SHA),
        cs(TLS_DH_anon_WITH_SEED_CBC_SHA, 0x009B, DH_anon, SEED_CBC, SHA),
        cs(TLS_RSA_WITH_AES_128_GCM_SHA256, 0x009C, RSA, AES_128_GCM, SHA256),
        cs(TLS_RSA_WITH_AES_256_GCM_SHA384, 0x009D, RSA, AES_256_GCM, SHA384),
        cs(TLS_DHE_RSA_WITH_AES_128_GCM_SHA256, 0x009E, DHE_RSA, AES_128_GCM, SHA256),
        cs(TLS_DHE_RSA_WITH_AES_256_GCM_SHA384, 0x009F, DHE_RSA, AES_256_GCM, SHA384),
        cs(TLS_DH_RSA_WITH_AES_128_GCM_SHA256, 0x00A0, DH_RSA, AES_128_GCM, SHA256),
        cs(TLS_DH_RSA_WITH_AES_256_GCM_SHA384, 0x00A1, DH_RSA, AES_256_GCM, SHA384),
        cs(TLS_DHE_DSS_WITH_AES_128_GCM_SHA256, 0x00A2, DHE_DSS, AES_128_GCM, SHA256),
        cs(TLS_DHE_DSS_WITH_AES_256_GCM_SHA384, 0x00A3, DHE_DSS, AES_256_GCM, SHA384),
        cs(TLS_DH_DSS_WITH_AES_128_GCM_SHA256, 0x00A4, DH_DSS, AES_128_GCM, SHA256),
        cs(TLS_DH_DSS_WITH_AES_256_GCM_SHA384, 0x00A5, DH_DSS, AES_256_GCM, SHA384),
        cs(TLS_DH_anon_WITH_AES_128_GCM_SHA256, 0x00A6, DH_anon, AES_128_GCM, SHA256),
        cs(TLS_DH_anon_WITH_AES_256_GCM_SHA384, 0x00A7, DH_anon, AES_256_GCM, SHA384),
        cs(TLS_PSK_WITH_AES_128_GCM_SHA256, 0x00A8, PSK, AES_128_GCM, SHA256),
        cs(TLS_PSK_WITH_AES_256_GCM_SHA384, 0x00A9, PSK, AES_256_GCM, SHA384),
        cs(TLS_DHE_PSK_WITH_AES_128_GCM_SHA256, 0x00AA, DHE_PSK, AES_128_GCM, SHA256),
        cs(TLS_DHE_PSK_WITH_AES_256_GCM_SHA384, 0x00AB, DHE_PSK, AES_256_GCM, SHA384),
        cs(TLS_RSA_PSK_WITH_AES_128_GCM_SHA256, 0x00AC, RSA_PSK, AES_128_GCM, SHA256),
        cs(TLS_RSA_PSK_WITH_AES_256_GCM_SHA384, 0x00AD, RSA_PSK, AES_256_GCM, SHA384),
        cs(TLS_PSK_WITH_AES_128_CBC_SHA256, 0x00AE, PSK, AES_128_CBC, SHA256),
        cs(TLS_PSK_WITH_AES_256_CBC_SHA384, 0x00AF, PSK, AES_256_CBC, SHA384),
        cs(TLS_PSK_WITH_NULL_SHA256, 0x00B0, PSK, NONE, SHA256),
        cs(TLS_PSK_WITH_NULL_SHA384, 0x00B1, PSK, NONE, SHA384),
        cs(TLS_DHE_PSK_WITH_AES_128_CBC_SHA256, 0x00B2, DHE_PSK, AES_128_CBC, SHA256),
        cs(TLS_DHE_PSK_WITH_AES_256_CBC_SHA384, 0x00B3, DHE_PSK, AES_256_CBC, SHA384),
        cs(TLS_DHE_PSK_WITH_NULL_SHA256, 0x00B4, DHE_PSK, NONE, SHA256),
        cs(TLS_DHE_PSK_WITH_NULL_SHA384, 0x00B5, DHE_PSK, NONE, SHA384),
        cs(TLS_RSA_PSK_WITH_AES_128_CBC_SHA256, 0x00B6, RSA_PSK, AES_128_CBC, SHA256),
        cs(TLS_RSA_PSK_WITH_AES_256_CBC_SHA384, 0x00B7, RSA_PSK, AES_256_CBC, SHA384),
        cs(TLS_RSA_PSK_WITH_NULL_SHA256, 0x00B8, RSA_PSK, NONE, SHA256),
        cs(TLS_RSA_PSK_WITH_NULL_SHA384, 0x00B9, RSA_PSK, NONE, SHA384),
        cs(TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256, 0x00BA, RSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA256, 0x00BB, DH_DSS, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA256, 0x00BC, DH_RSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA256, 0x00BD, DHE_DSS, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256, 0x00BE, DHE_RSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA256, 0x00BF, DH_anon, CAMELLIA_128_CBC, SHA256),
        cs(TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256, 0x00C0, RSA, CAMELLIA_256_CBC, SHA256),
        cs(TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA256, 0x00C1, DH_DSS, CAMELLIA_256_CBC, SHA256),
        cs(TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA256, 0x00C2, DH_RSA, CAMELLIA_256_CBC, SHA256),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA256, 0x00C3, DHE_DSS, CAMELLIA_256_CBC, SHA256),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256, 0x00C4, DHE_RSA, CAMELLIA_256_CBC, SHA256),
        cs(TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA256, 0x00C5, DH_anon, CAMELLIA_256_CBC, SHA256),
        cs(TLS_SM4_GCM_SM3, 0x00C6, UNKNOWN, SM4_GCM, SM3),
        cs(TLS_SM4_CCM_SM3, 0x00C7, UNKNOWN, SM4_CCM, SM3),
        cs(TLS_EMPTY_RENEGOTIATION_INFO_SCSV, 0x00FF, NONE, UNKNOWN, UNKNOWN),
        cs(TLS_AES_128_GCM_SHA256, 0x1301, UNKNOWN, AES_128_GCM, SHA256),
        cs(TLS_AES_256_GCM_SHA384, 0x1302, UNKNOWN, AES_256_GCM, SHA384),
        cs(TLS_CHACHA20_POLY1305_SHA256, 0x1303, UNKNOWN, CHACHA20, SHA256),
        cs(TLS_AES_128_CCM_SHA256, 0x1304, UNKNOWN, AES_128_CCM, SHA256),
        cs(TLS_AES_128_CCM_8_SHA256, 0x1305, UNKNOWN, AES_128_CCM_8, SHA256),
        cs(TLS_FALLBACK_SCSV, 0x5600, UNKNOWN, UNKNOWN, UNKNOWN),
        cs(TLS_ECDH_ECDSA_WITH_NULL_SHA, 0xC001, ECDH_ECDSA, NONE, SHA),
        cs(TLS_ECDH_ECDSA_WITH_RC4_128_SHA, 0xC002, ECDH_ECDSA, RC4_128, SHA),
        cs(TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA, 0xC003, ECDH_ECDSA, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA, 0xC004, ECDH_ECDSA, AES_128_CBC, SHA),
        cs(TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA, 0xC005, ECDH_ECDSA, AES_256_CBC, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_NULL_SHA, 0xC006, ECDHE_ECDSA, NONE, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_RC4_128_SHA, 0xC007, ECDHE_ECDSA, RC4_128, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA, 0xC008, ECDHE_ECDSA, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, 0xC009, ECDHE_ECDSA, AES_128_CBC, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA, 0xC00A, ECDHE_ECDSA, AES_256_CBC, SHA),
        cs(TLS_ECDH_RSA_WITH_NULL_SHA, 0xC00B, ECDH_RSA, NONE, SHA),
        cs(TLS_ECDH_RSA_WITH_RC4_128_SHA, 0xC00C, ECDH_RSA, RC4_128, SHA),
        cs(TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA, 0xC00D, ECDH_RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDH_RSA_WITH_AES_128_CBC_SHA, 0xC00E, ECDH_RSA, AES_128_CBC, SHA),
        cs(TLS_ECDH_RSA_WITH_AES_256_CBC_SHA, 0xC00F, ECDH_RSA, AES_256_CBC, SHA),
        cs(TLS_ECDHE_RSA_WITH_NULL_SHA, 0xC010, ECDHE_RSA, NONE, SHA),
        cs(TLS_ECDHE_RSA_WITH_RC4_128_SHA, 0xC011, ECDHE_RSA, RC4_128, SHA),
        cs(TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA, 0xC012, ECDHE_RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA, 0xC013, ECDHE_RSA, AES_128_CBC, SHA),
        cs(TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA, 0xC014, ECDHE_RSA, AES_256_CBC, SHA),
        cs(TLS_ECDH_anon_WITH_NULL_SHA, 0xC015, ECDH_anon, NONE, SHA),
        cs(TLS_ECDH_anon_WITH_RC4_128_SHA, 0xC016, ECDH_anon, RC4_128, SHA),
        cs(TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA, 0xC017, ECDH_anon, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDH_anon_WITH_AES_128_CBC_SHA, 0xC018, ECDH_anon, AES_128_CBC, SHA),
        cs(TLS_ECDH_anon_WITH_AES_256_CBC_SHA, 0xC019, ECDH_anon, AES_256_CBC, SHA),
        cs(TLS_SRP_SHA_WITH_3DES_EDE_CBC_SHA, 0xC01A, SRP_SHA, _3DES_EDE_CBC, SHA),
        cs(TLS_SRP_SHA_RSA_WITH_3DES_EDE_CBC_SHA, 0xC01B, SRP_SHA_RSA, _3DES_EDE_CBC, SHA),
        cs(TLS_SRP_SHA_DSS_WITH_3DES_EDE_CBC_SHA, 0xC01C, SRP_SHA_DSS, _3DES_EDE_CBC, SHA),
        cs(TLS_SRP_SHA_WITH_AES_128_CBC_SHA, 0xC01D, SRP_SHA, AES_128_CBC, SHA),
        cs(TLS_SRP_SHA_RSA_WITH_AES_128_CBC_SHA, 0xC01E, SRP_SHA_RSA, AES_128_CBC, SHA),
        cs(TLS_SRP_SHA_DSS_WITH_AES_128_CBC_SHA, 0xC01F, SRP_SHA_DSS, AES_128_CBC, SHA),
        cs(TLS_SRP_SHA_WITH_AES_256_CBC_SHA, 0xC020, SRP_SHA, AES_256_CBC, SHA),
        cs(TLS_SRP_SHA_RSA_WITH_AES_256_CBC_SHA, 0xC021, SRP_SHA_RSA, AES_256_CBC, SHA),
        cs(TLS_SRP_SHA_DSS_WITH_AES_256_CBC_SHA, 0xC022, SRP_SHA_DSS, AES_256_CBC, SHA),
        cs(TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, 0xC023, ECDHE_ECDSA, AES_128_CBC, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, 0xC024, ECDHE_ECDSA, AES_256_CBC, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256, 0xC025, ECDH_ECDSA, AES_128_CBC, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384, 0xC026, ECDH_ECDSA, AES_256_CBC, SHA384),
        cs(TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, 0xC027, ECDHE_RSA, AES_128_CBC, SHA256),
        cs(TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384, 0xC028, ECDHE_RSA, AES_256_CBC, SHA384),
        cs(TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256, 0xC029, ECDH_RSA, AES_128_CBC, SHA256),
        cs(TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384, 0xC02A, ECDH_RSA, AES_256_CBC, SHA384),
        cs(TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, 0xC02B, ECDHE_ECDSA, AES_128_GCM, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, 0xC02C, ECDHE_ECDSA, AES_256_GCM, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256, 0xC02D, ECDH_ECDSA, AES_128_GCM, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384, 0xC02E, ECDH_ECDSA, AES_256_GCM, SHA384),
        cs(TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, 0xC02F, ECDHE_RSA, AES_128_GCM, SHA256),
        cs(TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, 0xC030, ECDHE_RSA, AES_256_GCM, SHA384),
        cs(TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256, 0xC031, ECDH_RSA, AES_128_GCM, SHA256),
        cs(TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384, 0xC032, ECDH_RSA, AES_256_GCM, SHA384),
        cs(TLS_ECDHE_PSK_WITH_RC4_128_SHA, 0xC033, ECDHE_PSK, RC4_128, SHA),
        cs(TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA, 0xC034, ECDHE_PSK, _3DES_EDE_CBC, SHA),
        cs(TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA, 0xC035, ECDHE_PSK, AES_128_CBC, SHA),
        cs(TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA, 0xC036, ECDHE_PSK, AES_256_CBC, SHA),
        cs(TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256, 0xC037, ECDHE_PSK, AES_128_CBC, SHA256),
        cs(TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384, 0xC038, ECDHE_PSK, AES_256_CBC, SHA384),
        cs(TLS_ECDHE_PSK_WITH_NULL_SHA, 0xC039, ECDHE_PSK, NONE, SHA),
        cs(TLS_ECDHE_PSK_WITH_NULL_SHA256, 0xC03A, ECDHE_PSK, NONE, SHA256),
        cs(TLS_ECDHE_PSK_WITH_NULL_SHA384, 0xC03B, ECDHE_PSK, NONE, SHA384),
        cs(TLS_RSA_WITH_ARIA_128_CBC_SHA256, 0xC03C, RSA, ARIA_128_CBC, SHA256),
        cs(TLS_RSA_WITH_ARIA_256_CBC_SHA384, 0xC03D, RSA, ARIA_256_CBC, SHA384),
        cs(TLS_DH_DSS_WITH_ARIA_128_CBC_SHA256, 0xC03E, DH_DSS, ARIA_128_CBC, SHA256),
        cs(TLS_DH_DSS_WITH_ARIA_256_CBC_SHA384, 0xC03F, DH_DSS, ARIA_256_CBC, SHA384),
        cs(TLS_DH_RSA_WITH_ARIA_128_CBC_SHA256, 0xC040, DH_RSA, ARIA_128_CBC, SHA256),
        cs(TLS_DH_RSA_WITH_ARIA_256_CBC_SHA384, 0xC041, DH_RSA, ARIA_256_CBC, SHA384),
        cs(TLS_DHE_DSS_WITH_ARIA_128_CBC_SHA256, 0xC042, DHE_DSS, ARIA_128_CBC, SHA256),
        cs(TLS_DHE_DSS_WITH_ARIA_256_CBC_SHA384, 0xC043, DHE_DSS, ARIA_256_CBC, SHA384),
        cs(TLS_DHE_RSA_WITH_ARIA_128_CBC_SHA256, 0xC044, DHE_RSA, ARIA_128_CBC, SHA256),
        cs(TLS_DHE_RSA_WITH_ARIA_256_CBC_SHA384, 0xC045, DHE_RSA, ARIA_256_CBC, SHA384),
        cs(TLS_DH_anon_WITH_ARIA_128_CBC_SHA256, 0xC046, DH_anon, ARIA_128_CBC, SHA256),
        cs(TLS_DH_anon_WITH_ARIA_256_CBC_SHA384, 0xC047, DH_anon, ARIA_256_CBC, SHA384),
        cs(TLS_ECDHE_ECDSA_WITH_ARIA_128_CBC_SHA256, 0xC048, ECDHE_ECDSA, ARIA_128_CBC, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384, 0xC049, ECDHE_ECDSA, ARIA_256_CBC, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256, 0xC04A, ECDH_ECDSA, ARIA_128_CBC, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384, 0xC04B, ECDH_ECDSA, ARIA_256_CBC, SHA384),
        cs(TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256, 0xC04C, ECDHE_RSA, ARIA_128_CBC, SHA256),
        cs(TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384, 0xC04D, ECDHE_RSA, ARIA_256_CBC, SHA384),
        cs(TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256, 0xC04E, ECDH_RSA, ARIA_128_CBC, SHA256),
        cs(TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384, 0xC04F, ECDH_RSA, ARIA_256_CBC, SHA384),
        cs(TLS_RSA_WITH_ARIA_128_GCM_SHA256, 0xC050, RSA, ARIA_128_GCM, SHA256),
        cs(TLS_RSA_WITH_ARIA_256_GCM_SHA384, 0xC051, RSA, ARIA_256_GCM, SHA384),
        cs(TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256, 0xC052, DHE_RSA, ARIA_128_GCM, SHA256),
        cs(TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384, 0xC053, DHE_RSA, ARIA_256_GCM, SHA384),
        cs(TLS_DH_RSA_WITH_ARIA_128_GCM_SHA256, 0xC054, DH_RSA, ARIA_128_GCM, SHA256),
        cs(TLS_DH_RSA_WITH_ARIA_256_GCM_SHA384, 0xC055, DH_RSA, ARIA_256_GCM, SHA384),
        cs(TLS_DHE_DSS_WITH_ARIA_128_GCM_SHA256, 0xC056, DHE_DSS, ARIA_128_GCM, SHA256),
        cs(TLS_DHE_DSS_WITH_ARIA_256_GCM_SHA384, 0xC057, DHE_DSS, ARIA_256_GCM, SHA384),
        cs(TLS_DH_DSS_WITH_ARIA_128_GCM_SHA256, 0xC058, DH_DSS, ARIA_128_GCM, SHA256),
        cs(TLS_DH_DSS_WITH_ARIA_256_GCM_SHA384, 0xC059, DH_DSS, ARIA_256_GCM, SHA384),
        cs(TLS_DH_anon_WITH_ARIA_128_GCM_SHA256, 0xC05A, DH_anon, ARIA_128_GCM, SHA256),
        cs(TLS_DH_anon_WITH_ARIA_256_GCM_SHA384, 0xC05B, DH_anon, ARIA_256_GCM, SHA384),
        cs(TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256, 0xC05C, ECDHE_ECDSA, ARIA_128_GCM, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384, 0xC05D, ECDHE_ECDSA, ARIA_256_GCM, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256, 0xC05E, ECDH_ECDSA, ARIA_128_GCM, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384, 0xC05F, ECDH_ECDSA, ARIA_256_GCM, SHA384),
        cs(TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256, 0xC060, ECDHE_RSA, ARIA_128_GCM, SHA256),
        cs(TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384, 0xC061, ECDHE_RSA, ARIA_256_GCM, SHA384),
        cs(TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256, 0xC062, ECDH_RSA, ARIA_128_GCM, SHA256),
        cs(TLS_ECDH_RSA_WITH_ARIA_256_GCM_SHA384, 0xC063, ECDH_RSA, ARIA_256_GCM, SHA384),
        cs(TLS_PSK_WITH_ARIA_128_CBC_SHA256, 0xC064, PSK, ARIA_128_CBC, SHA256),
        cs(TLS_PSK_WITH_ARIA_256_CBC_SHA384, 0xC065, PSK, ARIA_256_CBC, SHA384),
        cs(TLS_DHE_PSK_WITH_ARIA_128_CBC_SHA256, 0xC066, DHE_PSK, ARIA_128_CBC, SHA256),
        cs(TLS_DHE_PSK_WITH_ARIA_256_CBC_SHA384, 0xC067, DHE_PSK, ARIA_256_CBC, SHA384),
        cs(TLS_RSA_PSK_WITH_ARIA_128_CBC_SHA256, 0xC068, RSA_PSK, ARIA_128_CBC, SHA256),
        cs(TLS_RSA_PSK_WITH_ARIA_256_CBC_SHA384, 0xC069, RSA_PSK, ARIA_256_CBC, SHA384),
        cs(TLS_PSK_WITH_ARIA_128_GCM_SHA256, 0xC06A, PSK, ARIA_128_GCM, SHA256),
        cs(TLS_PSK_WITH_ARIA_256_GCM_SHA384, 0xC06B, PSK, ARIA_256_GCM, SHA384),
        cs(TLS_DHE_PSK_WITH_ARIA_128_GCM_SHA256, 0xC06C, DHE_PSK, ARIA_128_GCM, SHA256),
        cs(TLS_DHE_PSK_WITH_ARIA_256_GCM_SHA384, 0xC06D, DHE_PSK, ARIA_256_GCM, SHA384),
        cs(TLS_RSA_PSK_WITH_ARIA_128_GCM_SHA256, 0xC06E, RSA_PSK, ARIA_128_GCM, SHA256),
        cs(TLS_RSA_PSK_WITH_ARIA_256_GCM_SHA384, 0xC06F, RSA_PSK, ARIA_256_GCM, SHA384),
        cs(TLS_ECDHE_PSK_WITH_ARIA_128_CBC_SHA256, 0xC070, ECDHE_PSK, ARIA_128_CBC, SHA256),
        cs(TLS_ECDHE_PSK_WITH_ARIA_256_CBC_SHA384, 0xC071, ECDHE_PSK, ARIA_256_CBC, SHA384),
        cs(TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256, 0xC072, ECDHE_ECDSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384, 0xC073, ECDHE_ECDSA, CAMELLIA_256_CBC, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256, 0xC074, ECDH_ECDSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384, 0xC075, ECDH_ECDSA, CAMELLIA_256_CBC, SHA384),
        cs(TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256, 0xC076, ECDHE_RSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384, 0xC077, ECDHE_RSA, CAMELLIA_256_CBC, SHA384),
        cs(TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256, 0xC078, ECDH_RSA, CAMELLIA_128_CBC, SHA256),
        cs(TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384, 0xC079, ECDH_RSA, CAMELLIA_256_CBC, SHA384),
        cs(TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC07A, RSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC07B, RSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC07C, DHE_RSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC07D, DHE_RSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DH_RSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC07E, DH_RSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DH_RSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC07F, DH_RSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_128_GCM_SHA256, 0xC080, DHE_DSS, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DHE_DSS_WITH_CAMELLIA_256_GCM_SHA384, 0xC081, DHE_DSS, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DH_DSS_WITH_CAMELLIA_128_GCM_SHA256, 0xC082, DH_DSS, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DH_DSS_WITH_CAMELLIA_256_GCM_SHA384, 0xC083, DH_DSS, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DH_anon_WITH_CAMELLIA_128_GCM_SHA256, 0xC084, DH_anon, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DH_anon_WITH_CAMELLIA_256_GCM_SHA384, 0xC085, DH_anon, CAMELLIA_256_GCM, SHA384),
        cs(TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC086, ECDHE_ECDSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC087, ECDHE_ECDSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC088, ECDH_ECDSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC089, ECDH_ECDSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC08A, ECDHE_RSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC08B, ECDHE_RSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256, 0xC08C, ECDH_RSA, CAMELLIA_128_GCM, SHA256),
        cs(TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384, 0xC08D, ECDH_RSA, CAMELLIA_256_GCM, SHA384),
        cs(TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256, 0xC08E, PSK, CAMELLIA_128_GCM, SHA256),
        cs(TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384, 0xC08F, PSK, CAMELLIA_256_GCM, SHA384),
        cs(TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256, 0xC090, DHE_PSK, CAMELLIA_128_GCM, SHA256),
        cs(TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384, 0xC091, DHE_PSK, CAMELLIA_256_GCM, SHA384),
        cs(TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256, 0xC092, RSA_PSK, CAMELLIA_128_GCM, SHA256),
        cs(TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384, 0xC093, RSA_PSK, CAMELLIA_256_GCM, SHA384),
        cs(TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256, 0xC094, PSK, CAMELLIA_128_CBC, SHA256),
        cs(TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384, 0xC095, PSK, CAMELLIA_256_CBC, SHA384),
        cs(TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256, 0xC096, DHE_PSK, CAMELLIA_128_CBC, SHA256),
        cs(TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384, 0xC097, DHE_PSK, CAMELLIA_256_CBC, SHA384),
        cs(TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256, 0xC098, RSA_PSK, CAMELLIA_128_CBC, SHA256),
        cs(TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384, 0xC099, RSA_PSK, CAMELLIA_256_CBC, SHA384),
        cs(TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256, 0xC09A, ECDHE_PSK, CAMELLIA_128_CBC, SHA256),
        cs(TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384, 0xC09B, ECDHE_PSK, CAMELLIA_256_CBC, SHA384),
        cs(TLS_RSA_WITH_AES_128_CCM, 0xC09C, RSA, AES_128_CCM, UNKNOWN),
        cs(TLS_RSA_WITH_AES_256_CCM, 0xC09D, RSA, AES_256_CCM, UNKNOWN),
        cs(TLS_DHE_RSA_WITH_AES_128_CCM, 0xC09E, DHE_RSA, AES_128_CCM, UNKNOWN),
        cs(TLS_DHE_RSA_WITH_AES_256_CCM, 0xC09F, DHE_RSA, AES_256_CCM, UNKNOWN),
        cs(TLS_RSA_WITH_AES_128_CCM_8, 0xC0A0, RSA, AES_128_CCM_8, UNKNOWN),
        cs(TLS_RSA_WITH_AES_256_CCM_8, 0xC0A1, RSA, AES_256_CCM_8, UNKNOWN),
        cs(TLS_DHE_RSA_WITH_AES_128_CCM_8, 0xC0A2, DHE_RSA, AES_128_CCM_8, UNKNOWN),
        cs(TLS_DHE_RSA_WITH_AES_256_CCM_8, 0xC0A3, DHE_RSA, AES_256_CCM_8, UNKNOWN),
        cs(TLS_PSK_WITH_AES_128_CCM, 0xC0A4, PSK, AES_128_CCM, UNKNOWN),
        cs(TLS_PSK_WITH_AES_256_CCM, 0xC0A5, PSK, AES_256_CCM, UNKNOWN),
        cs(TLS_DHE_PSK_WITH_AES_128_CCM, 0xC0A6, DHE_PSK, AES_128_CCM, UNKNOWN),
        cs(TLS_DHE_PSK_WITH_AES_256_CCM, 0xC0A7, DHE_PSK, AES_256_CCM, UNKNOWN),
        cs(TLS_PSK_WITH_AES_128_CCM_8, 0xC0A8, PSK, AES_128_CCM_8, UNKNOWN),
        cs(TLS_PSK_WITH_AES_256_CCM_8, 0xC0A9, PSK, AES_256_CCM_8, UNKNOWN),
        cs(TLS_PSK_DHE_WITH_AES_128_CCM_8, 0xC0AA, PSK_DHE, AES_128_CCM_8, UNKNOWN),
        cs(TLS_PSK_DHE_WITH_AES_256_CCM_8, 0xC0AB, PSK_DHE, AES_256_CCM_8, UNKNOWN),
        cs(TLS_ECDHE_ECDSA_WITH_AES_128_CCM, 0xC0AC, ECDHE_ECDSA, AES_128_CCM, UNKNOWN),
        cs(TLS_ECDHE_ECDSA_WITH_AES_256_CCM, 0xC0AD, ECDHE_ECDSA, AES_256_CCM, UNKNOWN),
        cs(TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8, 0xC0AE, ECDHE_ECDSA, AES_128_CCM_8, UNKNOWN),
        cs(TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8, 0xC0AF, ECDHE_ECDSA, AES_256_CCM_8, UNKNOWN),
        cs(TLS_ECCPWD_WITH_AES_128_GCM_SHA256, 0xC0B0, ECCPWD, AES_128_GCM, SHA256),
        cs(TLS_ECCPWD_WITH_AES_256_GCM_SHA384, 0xC0B1, ECCPWD, AES_256_GCM, SHA384),
        cs(TLS_ECCPWD_WITH_AES_128_CCM_SHA256, 0xC0B2, ECCPWD, AES_128_CCM, SHA256),
        cs(TLS_ECCPWD_WITH_AES_256_CCM_SHA384, 0xC0B3, ECCPWD, AES_256_CCM, SHA384),
        cs(TLS_SHA256_SHA256, 0xC0B4, UNKNOWN, UNKNOWN, SHA256),
        cs(TLS_SHA384_SHA384, 0xC0B5, UNKNOWN, UNKNOWN, SHA384),
        cs(TLS_GOSTR341112_256_WITH_KUZNYECHIK_CTR_OMAC, 0xC100, GOSTR341112_256, KUZNYECHIK_CTR, OMAC),
        cs(TLS_GOSTR341112_256_WITH_MAGMA_CTR_OMAC, 0xC101, GOSTR341112_256, MAGMA_CTR, OMAC),
        cs(TLS_GOSTR341112_256_WITH_28147_CNT_IMIT, 0xC102, GOSTR341112_256, _28147_CNT, IMIT),
        cs(TLS_GOSTR341112_256_WITH_KUZNYECHIK_MGM_L, 0xC103, GOSTR341112_256, KUZNYECHIK_MGM, MGM_L),
        cs(TLS_GOSTR341112_256_WITH_MAGMA_MGM_L, 0xC104, GOSTR341112_256, MAGMA, MGM_L),
        cs(TLS_GOSTR341112_256_WITH_KUZNYECHIK_MGM_S, 0xC105, GOSTR341112_256, KUZNYECHIK, MGM_S),
        cs(TLS_GOSTR341112_256_WITH_MAGMA_MGM_S, 0xC106, GOSTR341112_256, MAGMA, MGM_S),
        cs(TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCA8, ECDHE_RSA, CHACHA20_POLY1305, SHA256),
        cs(TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCA9, ECDHE_ECDSA, CHACHA20_POLY1305, SHA256),
        cs(TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCAA, DHE_RSA, CHACHA20_POLY1305, SHA256),
        cs(TLS_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAB, PSK, CHACHA20_POLY1305, SHA256),
        cs(TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAC, ECDHE_PSK, CHACHA20_POLY1305, SHA256),
        cs(TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAD, DHE_PSK, CHACHA20_POLY1305, SHA256),
        cs(TLS_RSA_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAE, RSA_PSK, CHACHA20_POLY1305, SHA256),
        cs(TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256, 0xD001, ECDHE_PSK, AES_128_GCM, SHA256),
        cs(TLS_ECDHE_PSK_WITH_AES_256_GCM_SHA384, 0xD002, ECDHE_PSK, AES_256_GCM, SHA384),
        cs(TLS_ECDHE_PSK_WITH_AES_128_CCM_8_SHA256, 0xD003, ECDHE_PSK, AES_128_CCM_8, SHA256),
        cs(TLS_ECDHE_PSK_WITH_AES_128_CCM_SHA256, 0xD005, ECDHE_PSK, AES_128_CCM, SHA256),
    };
    #undef cs
    // just checking and matching
    for (auto &cs : cipher_suites)
        if (cs.value == key)
            return cs;
    throw CapException("unknown or reserved cipher suite");
}