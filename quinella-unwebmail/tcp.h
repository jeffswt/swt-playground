
#ifndef _QUINELLA_TCP_H
#define _QUINELLA_TCP_H

#include <map>
#include <nids.h>

#include "packets.h"


/// Tuple of IPv4 address and port. There are certain public attributes which
/// you really shouldn't touch.
class IpPort {
public:
    uint32_t addr;
    uint16_t port;
    IpPort();
    IpPort(uint32_t addr, uint16_t port);
    bool operator == (const IpPort &other) const;
    bool operator < (const IpPort &other) const;
    /// Convert (IP, port) to dot-decimal:port notation. Example: 127.0.0.1:80
    std::string to_string();
};

std::ostream& operator << (std::ostream &stream, IpPort ipp);

/// (src_ip, src_port, dest_ip, dest_port) uniquely determines a TCP stream.
/// It's basically an alias of an IpPort pair.
typedef std::pair<IpPort, IpPort> ConnTuple;

std::ostream& operator << (std::ostream &stream, ConnTuple tuple);

/// Converts libnids tuple4 to ConnTuple. They are basically the same.
/// @param tuple: libnids tuple.
/// @return ConnTuple object.
ConnTuple tuple4_to_conn(tuple4 tuple);

/// A complete TCP stream, containing outbound (leaving localhost) and inbound
/// (entering localhost) data (packets doesn't matter because it's TCP).
class TcpStream {
public:
    /// TCP connection endpoints.
    ConnTuple conn;
    /// Data leaving from localhost.
    std::string outbound;
    /// Data entering into localhost.
    std::string inbound;
    TcpStream();
    TcpStream(ConnTuple conn);
};

/// TCP stream handler. Function should respond to TcpStream types.
typedef void (*TcpStreamHandler)(TcpStream);

/// Start TCP stream capture with given handler.
/// This function is not thread safe. Use in single-threaded environment only.
/// @param device: PcapDevice retrieved with pcap_get_all_devices().
/// @param filter: Packet capture filter. See man 7 pcap-filter for details.
/// @param handler: TcpStream handler.
/// @exception Undefined behaviour when used in multithreading environments.
void tcp_capture_stream(PcapDevice device, std::string filter,
                        TcpStreamHandler handler);

#endif  // _QUINELLA_TCP_H
