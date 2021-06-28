
#include <memory>
#include <pcap/pcap.h>

#include "utils.h"


/// Abstracted libpcap device that allows C++-style operations.
class PcapDevice {
private:
    pcap_if_t *_ptr;
    std::shared_ptr<pcap_if_t> _ref;
public:
    /// Retrieve delegated pointer.
    /// @return Retrieved pointer should not have a lifetime longer than the
    ///         PcapDevice itself.
    pcap_if_t* ptr();
    /// Device name, like "eth0".
    std::string name();
    /// Description, like "loopback device".
    std::string description();
    /// A list of pcap_addr* addresses bound by this interface.
    std::vector<pcap_addr*> addresses();
    /// Get flags.
    bpf_u_int32 flags();
    PcapDevice();
    PcapDevice(pcap_if_t *ptr, std::shared_ptr<pcap_if_t> ref);
};

/// libpcap packet, alias for std::string.
typedef std::string PcapPacket;

/// Get list of network interfaces local to this computer.
/// @return A list of PcapDevice's, each of them can be accessed through C++
///         interfaces, and can be treated with RAII perfectly. Lifetime
///         is handled via a delegate so memory leaks won't happen.
std::vector<PcapDevice> pcap_get_all_devices();

/// Opens libpcap packet capture on the given device with filter.
/// @param device: Target PcapDevice to use. This can be get with function
///                pcap_get_all_devices().
/// @param filter: Packet capture filter. See man 7 pcap-filter for details.
/// @return pcap_t* structure initialized to target device with given filter
///         used to open libpcap capturing.
pcap_t* pcap_open_capture_delegate(PcapDevice device, std::string filter);

/// Run libpcap capture on target device with filter, given handler.
/// @param device: Target PcapDevice to use. Get with pcap_get_all_devices().
/// @param filter: Packet capture filter. See man 7 pcap-filter for details.
/// @param handle: Handler function which responds to PcapPacket's. Function
///                signature should be like "void myHandler(PcapPacket pkt)".
void pcap_capture_tcp_packets(PcapDevice device, std::string filter,
                              void (*handler)(PcapPacket));
