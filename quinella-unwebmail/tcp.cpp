
#include "tcp.h"

#include <sstream>


IpPort::IpPort() : addr(0), port(0) {}

IpPort::IpPort(uint32_t addr, uint16_t port) : addr(addr), port(port) {}

bool IpPort::operator == (const IpPort &other) const {
    return this->addr == other.addr && this->port == other.port;
}

bool IpPort::operator < (const IpPort &other) const {
    if (this->addr == other.addr)
        return this->port < other.port;
    return this->addr < other.addr;
}

std::string IpPort::to_string() {
    std::stringstream ss;
    ss << (addr & 0xff) << ".";
    ss << ((addr >> 8) & 0xff) << ".";
    ss << ((addr >> 16) & 0xff) << ".";
    ss << ((addr >> 24) & 0xff) << ":";
    ss << port;
    std::string out;
    std::getline(ss, out);
    return out;
}

std::ostream& operator << (std::ostream &stream, IpPort ipp) {
    stream << ipp.to_string();
    return stream;
}

std::ostream& operator << (std::ostream &stream, ConnTuple tuple) {
    stream << "(" << tuple.first << " -> " << tuple.second << ")";
    return stream;
}

ConnTuple tuple4_to_conn(tuple4 tuple) {
    return std::make_pair(
        IpPort(tuple.saddr, tuple.source),
        IpPort(tuple.daddr, tuple.dest)
    );
}

TcpStream::TcpStream() : conn() {}

TcpStream::TcpStream(ConnTuple conn) : conn(conn) {}

/// Buffer of TcpStream's. Cleanup entry whenever TCP stream terminates.
static std::map<ConnTuple, TcpStream> _tcp_stream_buffers;

/// TCP stream handler. This is shared among threads and is not thread-safe.
static TcpStreamHandler _tcp_stream_handler = nullptr;

/// libnids TCP packet callback. This forwards data to the buffers thus
/// enabling TCP stream handlers to receive more readable data.
void _nids_tcp_callback(tcp_stream *a_tcp, void **param) {
    ConnTuple conn = tuple4_to_conn(a_tcp->addr);
    auto nids_state = a_tcp->nids_state;
    // judge nids state
    if (nids_state == NIDS_JUST_EST) {
        // inform libnids to capture this stream upon establishment
        a_tcp->client.collect += 1;
        a_tcp->server.collect += 1;
        a_tcp->client.collect_urg += 1;
        a_tcp->server.collect_urg += 1;
        // create entry in buffer
        _tcp_stream_buffers[conn] = TcpStream(conn);
    } else if (nids_state == NIDS_CLOSE || nids_state == NIDS_RESET) {
        // take out stream and call handler
        TcpStream stream = _tcp_stream_buffers[conn];
        _tcp_stream_buffers.erase(conn);
        if (_tcp_stream_handler)
            _tcp_stream_handler(stream);
    } else if (nids_state == NIDS_DATA) {
        // update data in stream
        TcpStream stream = _tcp_stream_buffers[conn];
        if (a_tcp->server.count_new_urg)
            stream.outbound += a_tcp->server.urgdata;
        for (int i = 0; i < a_tcp->server.count_new; i++)
            stream.outbound += a_tcp->server.data[i];
        if (a_tcp->client.count_new_urg)
            stream.inbound += a_tcp->client.urgdata;
        for (int i = 0; i < a_tcp->client.count_new; i++)
            stream.inbound += a_tcp->client.data[i];
        _tcp_stream_buffers[conn] = stream;
    }
    return ;
}

/// nids_pcap_handler(...) delegate that removes const requirements.
void _nids_pcap_handler(u_char *a, const pcap_pkthdr *b, const u_char *c) {
    // magic that converts const to non-const
    const intptr_t _b = (intptr_t)b, _c = (intptr_t)c;
    nids_pcap_handler(a, (pcap_pkthdr*)_b, (u_char*)_c);
}

void capture_tcp_stream(PcapDevice device, std::string filter,
                        TcpStreamHandler handler) {
    nids_params.promisc = false;
    if (!nids_init())
        throw CapException(nids_errbuf);
    // disable tcp checksum
    nids_chksum_ctl no_chksum[1];
    no_chksum[0].netaddr = 0x00000000;
    no_chksum[0].mask = 0x00000000;
    no_chksum[0].action = NIDS_DONT_CHKSUM;
    nids_register_chksum_ctl(no_chksum, 1);
    // capture packets with defined handler
    nids_register_tcp((void*)_nids_tcp_callback);
    _tcp_stream_handler = handler;
    pcap_t *handle = pcap_open_capture_delegate(device, filter);
    pcap_loop(handle, 0, _nids_pcap_handler, nullptr);
}
