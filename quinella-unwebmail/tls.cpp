
#include "tls.h"

#include <fstream>
#include "deps/unprettysoup/unprettysoup.h"

using namespace std;
using namespace us3;


vector<TlsRecord> tls_stream_from_tcp_stream(string stream) {
    // splits tcp stream into tls records
    // https://en.wikipedia.org/wiki/Transport_Layer_Security#TLS_record
    vector<TlsRecord> result;
    for (int pos = 0; pos + 4 < stream.length(); ) {
        // parse header
        TlsRecord cur;
        int startpos = pos;
        cur.content_type = stream[pos];
        cur.ver_major = stream[pos + 1];
        cur.ver_minor = stream[pos + 2];
        uint16_t length = (uint16_t)stream[pos + 3] & 0xff;
        length <<= 8;
        length += (uint16_t)stream[pos + 4] & 0xff;
        // parse body
        cur.message = "";
        pos += 5;
        for (int i = 0; i < length && pos + i < stream.length(); i++)
            cur.message += stream[pos + i];
        pos += length;
        result.push_back(cur);
    }
    return result;
}

TlsStream::TlsStream() {}

TlsStream::TlsStream(ConnTuple conn) : conn(conn) {}

/// Static variable for SSLKEYLOGFILE path. Not thread-safe.
static string _tls_sslkeylogfile_path = "";

/// Retrieve CLIENT_RANDOM or SERVER_RANDOM from TLS packet stream.
/// @param records: Reference to TLS packet stream.
/// @param content_type: Will only search for TLS records matching this certain
///                      content type, e.g. TLS_REC_CTYPE_Handshake.
/// @param handshake_type: Will only search for handshake records matching this
///                        criteria, e.g. TLS_REC_HSMTYPE_ClientHello.
/// @param get_cipher_suite: When cipher suite extraction is needed, store it
///                          in where this points to. nullptr otherwise.
/// @return Corresponding RANDOM in binary format.
string _tls_get_random_from_records(const vector<TlsRecord> &records,
        const uint8_t content_type, const uint8_t handshake_type,
        uint16_t *get_cipher_suite) {
    string result;
    for (const TlsRecord &rec : records) {
        if (rec.content_type != content_type)
            continue;
        uint8_t hsk_type = rec.message[0];
        if (hsk_type != handshake_type)
            continue;
        // get handshake message length
        uint32_t len_a = rec.message[1] & 0xff, len_b = rec.message[2] & 0xff,
            len_c = rec.message[3] & 0xff;
        uint32_t msglen = (len_a << 16) + (len_b << 8) + len_c;
        // handshake protocol version
        uint8_t hsk_major = rec.message[4], hsk_minor = rec.message[5];
        // client random, use last
        result.clear();
        for (int i = 0; i < 32; i++)
            result += rec.message[6 + i];
        // read more only if cipher suite required
        if (!get_cipher_suite)
            continue;
        int session_id_len = (int)rec.message[38] & 0xff;
        uint16_t cs_a = rec.message[39 + session_id_len] & 0xff,
            cs_b = rec.message[39 + session_id_len + 1] & 0xff;
        *get_cipher_suite = (cs_a << 8) + cs_b;
    }
    return result;
}

string tls_get_master_by_random(string random) {
    ifstream fin(_tls_sslkeylogfile_path);
    string buffer;
    if (!fin)
        throw CapException("Unable to open SSLKEYLOGFILE");
    while (getline(fin, buffer)) {
        String line(buffer);
        // https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format
        // Comment lines begin with a sharp character ('#') and are ignored.
        if (line.startswith("#"))
            continue;
        vector<String> words = line.split(" ");
        // filter only client random
        if (words[0] != "CLIENT_RANDOM")
            continue;
        // try and match
        string key = b16decode(words[1].to_string());
        if (key == random)
            return b16decode(words[2].to_string());
    }
    throw CapException("No master secret found matching given random");
}

void tls_set_keylogfile_path(string path) {
    _tls_sslkeylogfile_path = path;
    return ;
}

TlsStream tls_stream_from_tcp_stream(TcpStream stream) {
    // github:droe/sslsplit:main.c:481
    // github:droe/sslsplit:log.c:1590
    // github:droe/sslsplit:opts.c:1195
    // github:droe/sslsplit:pxyconn.c:2117 -> ssl_ssl_masterkey_to_str
    // github:droe/sslsplit:ssl.c:595
    // https://datatracker.ietf.org/doc/html/rfc5246#section-6.1
    TlsStream result;
    result.conn = stream.conn;
    result.outbound = tls_stream_from_tcp_stream(stream.outbound);
    result.inbound = tls_stream_from_tcp_stream(stream.inbound);
    // extract features from client / server hello -> client / server random
    result.client_random = _tls_get_random_from_records(
        result.outbound,
        TLS_REC_CTYPE_Handshake,
        TLS_REC_HSMTYPE_ClientHello,
        nullptr
    );
    uint16_t cipher_suite;
    result.server_random = _tls_get_random_from_records(
        result.inbound,
        TLS_REC_CTYPE_Handshake,
        TLS_REC_HSMTYPE_ServerHello,
        &cipher_suite
    );
    // load cipher suite
    result.cipher_suite = cipher_suite;
    // extract master secret from SSLKEYLOGFILE
    result.master_secret = tls_get_master_by_random(result.client_random);
    // done all, preparing for decryption
    return result;
}
