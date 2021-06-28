
#include <iostream>
#include "tcp.h"
#include <vector>


/// TLS record content types
const uint8_t TLS_REC_CTYPE_ChangeCipherSpec = 0x14;
const uint8_t TLS_REC_CTYPE_Alert = 0x15;
const uint8_t TLS_REC_CTYPE_Handshake = 0x16;
const uint8_t TLS_REC_CTYPE_Application = 0x17;
const uint8_t TLS_REC_CTYPE_Heartbeat = 0x18;

/// TLS handshake message types
const uint8_t TLS_REC_HSMTYPE_HelloRequest = 0;
const uint8_t TLS_REC_HSMTYPE_ClientHello = 1;
const uint8_t TLS_REC_HSMTYPE_ServerHello = 2;
const uint8_t TLS_REC_HSMTYPE_NewSessionTicket = 4;
const uint8_t TLS_REC_HSMTYPE_EncryptedExtensions = 8;
const uint8_t TLS_REC_HSMTYPE_Certificate = 11;
const uint8_t TLS_REC_HSMTYPE_ServerKeyExchange = 12;
const uint8_t TLS_REC_HSMTYPE_CertificateRequest = 13;
const uint8_t TLS_REC_HSMTYPE_ServerHelloDone = 14;
const uint8_t TLS_REC_HSMTYPE_CertificateVerify = 15;
const uint8_t TLS_REC_HSMTYPE_ClientKeyExchange = 16;
const uint8_t TLS_REC_HSMTYPE_Finished = 20;

/// A single application layer TLS packet.
class TlsRecord {
public:
    uint8_t content_type;
    uint8_t ver_major;
    uint8_t ver_minor;
    std::string message;
};

/// Strips regular TCP stream into list of TLS packets.
/// @param stream: Input TCP stream.
/// @return List of TlsRecord's. If the last bytes do not form a valid TLS
///         header, they will be silently ignored.
std::vector<TlsRecord> tls_stream_from_tcp_stream(std::string stream);

/// Full-duplex connection of TLS packets.
class TlsStream {
public:
    /// Connection endpoints descriptor.
    ConnTuple conn;
    /// Client-side random value.
    std::string client_random;
    /// Server-side random value.
    std::string server_random;
    /// Master secret retrieved from local SSLKEYLOGFILE.
    std::string master_secret;
    /// Session cipher suite ID.
    uint16_t cipher_suite;
    /// Outbound (leaving localhost) TLS packets.
    std::vector<TlsRecord> outbound;
    /// Inbound (coming to localhost) TLS packets.
    std::vector<TlsRecord> inbound;
    TlsStream();
    TlsStream(ConnTuple conn);
};

/// Retrieve MASTER_KEY from SSLKEYLOGFILE with CLIENT_RANDOM.
/// @param random: CLIENT_RANDOM in binary format.
/// @return MASTER_KEY in binary format.
std::string tls_get_master_by_random(std::string random);

/// Set SSLKEYLOGFILE path.
/// @param path: SSL key log file path.
void tls_set_keylogfile_path(std::string path);

/// Convert TCP stream to duplex TLS packets. Not decrypting by the way.
/// @param stream: Input TCP stream.
/// @return Exported TLS stream which is not yet decrypted.
TlsStream tls_stream_from_tcp_stream(TcpStream stream);
