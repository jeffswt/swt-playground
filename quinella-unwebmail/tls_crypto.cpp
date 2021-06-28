
#include "tls_crypto.h"

#include "utils.h"

using namespace std;


string tls_key_hmachash(CipherHmacProtocol hmac, string secret, string seed) {
    // https://datatracker.ietf.org/doc/html/rfc2104#section-2
    // https://www.openssl.org/docs/man1.1.1/man3/HMAC.html
    // https://www.openssl.org/docs/man1.1.1/man3/EVP_sha256.html
    // copy secret and seed
    char *key = new char[secret.length()];
    for (int i = 0; i < secret.length(); i++)
        key[i] = secret[i];
    unsigned char *d = new unsigned char[seed.length()];
    for (int i = 0; i < seed.length(); i++)
        d[i] = seed[i];
    // use hmac function
    unsigned char *result = new unsigned char[EVP_MAX_MD_SIZE];
    unsigned int result_len = 0;
    #define conj(chp, func)  if (hmac == CipherHmacProtocol::TLS_CHP_##chp)   \
        HMAC(func(), key, secret.length(), d, seed.length(), result,          \
            &result_len)
    conj(MD5, EVP_md5);
    conj(SHA, EVP_sha1);
    conj(SHA256, EVP_sha256);
    conj(SHA384, EVP_sha384);
    conj(SM3, EVP_sm3);
    #undef conj
    // dump data to result
    string sresult;
    for (int i = 0; i < result_len; i++)
        sresult += result[i];
    // clean memory
    delete[] key, d, result;
    // return
    if (result_len == 0)
        throw CapException("unsupported hmac protocol");
    return sresult;
}

string tls_key_phash(CipherHmacProtocol hmac, string secret, string seed,
        int length) {
    /// https://datatracker.ietf.org/doc/html/rfc5246#section-5
    /// First, we define a data expansion function, P_hash(secret, data), that
    ///  uses a single hash function to expand a secret and seed into an
    ///  arbitrary quantity of output:
    ///     P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
    ///                            HMAC_hash(secret, A(2) + seed) +
    ///                            HMAC_hash(secret, A(3) + seed) + ...
    /// where + indicates concatenation.
    ///     A() is defined as:
    ///      A(0) = seed
    ///      A(i) = HMAC_hash(secret, A(i-1))
    /// P_hash can be iterated as many times as necessary to produce the
    ///  required quantity of data.  For example, if P_SHA256 is being used to
    ///  create 80 bytes of data, it will have to be iterated three times
    ///  (through A(3)), creating 96 bytes of output data; the last 16 bytes of
    ///  the final iteration will then be discarded, leaving 80 bytes of output
    ///  data.
    string buffer;
    string A = seed;  // A was A[i-1], is A[i] after iteration
    for (int i = 1; buffer.length() < length; i++) {
        A = tls_key_hmachash(hmac, secret, A);
        buffer += tls_key_hmachash(hmac, secret, A + seed);
    }
    // take only [length] chars
    string result;
    for (int i = 0; i < length; i++)
        result += buffer[i];
    return result;
}

string tls_key_prf(CipherHmacProtocol hmac, string secret, string label,
        string seed, int length) {
    // https://datatracker.ietf.org/doc/html/rfc5246#section-5
    // TLS's PRF is created by applying P_hash to the secret as:
    //     PRF(secret, label, seed) = P_<hash>(secret, label + seed)
    // The label is an ASCII string.  It should be included in the exact form
    //  it is given without a length byte or trailing null character. For
    //  example, the label "slithy toves" would be processed by hashing the
    //  following bytes:
    //     73 6C 69 74 68 79 20 74 6F 76 65 73
    return tls_key_phash(hmac, secret, label + seed, length);
}
