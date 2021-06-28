
#include "packets.h"

#include <netinet/if_ether.h>


pcap_if_t* PcapDevice::ptr() {
    return _ptr;
}

std::string PcapDevice::name() {
    return _ptr->name ? _ptr->name : "";
}

std::string PcapDevice::description() {
    return _ptr->description ? _ptr->description : "";
}

std::vector<pcap_addr*> PcapDevice::addresses() {
    std::vector<pcap_addr*> res;
    for (auto q = _ptr->addresses; q; q = q->next)
        res.push_back(q);
    return res;
}

bpf_u_int32 PcapDevice::flags() {
    return _ptr->flags;
}

PcapDevice::PcapDevice() : _ptr(nullptr), _ref(nullptr) {}

PcapDevice::PcapDevice(pcap_if_t *ptr, std::shared_ptr<pcap_if_t> ref)
    : _ptr(ptr), _ref(ref) {}

/// Required to delete pcap_if_t struct upon lifetime excession.
class PcapIfTDeleter {
public:
    void operator()(pcap_if_t *ptr) {
        pcap_freealldevs(ptr);
    }
};

std::vector<PcapDevice> pcap_get_all_devices() {
    // retrieve from libpcap
    char err_buffer[PCAP_ERRBUF_SIZE];
    pcap_if_t* head;
    if (pcap_findalldevs(&head, err_buffer) != 0)
        throw CapException(err_buffer);
    // create lifetime handler
    auto ref = std::shared_ptr<pcap_if_t>(head, PcapIfTDeleter());
    // iterate all retrieved nodes
    std::vector<PcapDevice> devices;
    for (pcap_if_t* p = head; p; p = p->next)
        devices.push_back(PcapDevice(p, ref));
    return devices;
}

pcap_t* pcap_open_capture_delegate(PcapDevice device, std::string filter) {
    // open packet capturer
    char err_buffer[PCAP_ERRBUF_SIZE];
    const char *device_name = device.name().c_str();
    pcap_t *handle = pcap_open_live(
        device_name,
        BUFSIZ,
        false,  // do not open promiscuous mode
        150,  // notify the process every ... ms
        err_buffer
    );
    if (!handle)
        throw CapException(err_buffer);
    // retrieve ip and subnet mask
    bpf_u_int32 ip = 0, subnet_mask = 0;
    if (pcap_lookupnet(device_name, &ip, &subnet_mask, err_buffer) == -1)
        ip = subnet_mask = 0;
    // add packet filter
    bpf_program filterh;
    if (pcap_compile(handle, &filterh, filter.c_str(), 0, subnet_mask) == -1)
        throw CapException(pcap_geterr(handle));
    if (pcap_setfilter(handle, &filterh) == -1)
        throw CapException(pcap_geterr(handle));
    return handle;
}

void pcap_capture_tcp_packets(PcapDevice device, std::string filter,
                              void (*handler)(PcapPacket)) {
    pcap_t *handle = pcap_open_capture_delegate(device, filter);
    // get packet flow
    while (true) {
        pcap_pkthdr packet_header;
        const u_char *packet = pcap_next(handle, &packet_header);
        // filter only eth / ip / tcp packets
        ether_header *eth_hdr = (ether_header*)packet;
        if (ntohs(eth_hdr->ether_type) != ETHERTYPE_IP)
            continue;
        int eth_hdr_len = 14;
        const u_char *ip_hdr = packet + eth_hdr_len;
        int ip_hdr_len = ((*ip_hdr) & 0x0f) * 4;
        u_char proto = *(ip_hdr + 9);
        if (proto != IPPROTO_TCP)
            continue;
        // download packet and send to handler
        std::string packet_s;
        for (int i = 0; i < packet_header.len; i++)
            packet_s += (char)*(packet + i);
        handler(packet_s);
    }
    return ;
}
