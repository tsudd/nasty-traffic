#include "pcapworker/pcapworker.hpp"
#define HAVE_REMOTE
#include <pcap.h>
#include <sys/types.h>
#include <winsock2.h>
#include <time.h>

#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN	6
#define SNAP_LEN 65536

// Data labels
#define IPSRC_LBL "Source IP\n"
#define IPDST_LBL "Destination IP\n"
#define SRC_PORT_LBL "Source port\n"
#define DST_PORT_LBL "Destination port\n"
#define PROTOCOL_LBL "Protocol\n"
#define PLSIZE_LBL "Payload size\n"

/* Ethernet header */
struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off;                 /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* don't fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;               /* source port */
    u_short th_dport;               /* destination port */
    tcp_seq th_seq;                 /* sequence number */
    tcp_seq th_ack;                 /* acknowledgement number */
    u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
    u_char  th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;                 /* window */
    u_short th_sum;                 /* checksum */
    u_short th_urp;                 /* urgent pointer */
};

void get_devices_info(int& amount, std::vector<Device*>& devices) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    Device* dev;

    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        return;
    }

    for(d=alldevs; d; d=d->next)
    {
        i++;
        dev = new Device();
        dev->number = i;
        dev->name = std::string(d->name);
        if (d->description) dev->description = std::string(d->description);
        devices.push_back(dev);
    }
    amount = i;
}

void read_device_live(const char* dev, std::vector<PacketInfo*>& packets, const bool& flag, int port = -1) {
    pcap_t* handle;
    static int count = 0;
    char errbuf[PCAP_ERRBUF_SIZE];

    char* filter_exp = NULL;
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;

    pcap_pkthdr *header;
    const u_char *pkt_data;

    int i = 0;
    int res;

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        net = 0;
        mask = 0;
        // TODO: make exact exception
        throw std::exception();
    }

    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        // TODO: make exact exception
        throw std::exception();
    }
    if (port > 0) {
        sprintf(filter_exp, "ip port %d", port);
        if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
            // TODO: make exact exception
            throw std::exception();
        }

        if (pcap_setfilter(handle, &fp) == -1) {
            // TODO: make exact exception
            throw std::exception();
        }
    }
    time_t rawtime;
    tm* timeinfo;
    while((res = pcap_next_ex(handle, &header, &pkt_data)) >= 0 and flag) {
        static int count = 1;
        if (res == 0) {
            continue;
        }
        const sniff_ethernet* ethernet;
        const sniff_ip* ip;
        const sniff_tcp* tcp;
        const char* payload;

        int size_ip;
        int size_tcp;
        int size_payload;

        PacketInfo* packet = new PacketInfo();

        ethernet = (sniff_ethernet*)(pkt_data);

        ip = (sniff_ip*)(pkt_data + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20) {
            continue;
        }

        sprintf(packet->ip_src, "%s", inet_ntoa(ip->ip_src));
        sprintf(packet->ip_dst, "%s", inet_ntoa(ip->ip_dst));
        // protocol type
        switch(ip->ip_p) {
            case IPPROTO_TCP:
                sprintf(packet->protocol, "%s", "TCP");
                break;
            case IPPROTO_UDP:
                delete packet;
//                sprintf(packet->protocol, "%s", "UDP");
                continue;
            case IPPROTO_ICMP:
                delete packet;
//                sprintf(packet->protocol, "%s", "ICMP");
                continue;
            case IPPROTO_IP:
                delete packet;
//                sprintf(packet->protocol, "%s", "IP");
                continue;
            default:
                delete packet;
//                sprintf(packet->protocol, "%s", "unknown");
                continue;
        }

        count++;

        packet->number = count;

        rawtime = header->ts.tv_sec;
        timeinfo=localtime(&rawtime);
        strftime(packet->timestamp, 26, "%Y-%m-%d %H:%M:%S", timeinfo);

        tcp = (sniff_tcp*)(pkt_data + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp)*4;
        if (size_tcp < 20) {
            continue;
        }

        // ports info
        sprintf(packet->sport, "%d", ntohs(tcp->th_sport));
        sprintf(packet->dport, "%d", ntohs(tcp->th_dport));

//        payload = (char *)(pkt_data + SIZE_ETHERNET + size_ip + size_tcp);
        size_payload = ntohl(ip->ip_len) - (size_ip + size_tcp);

//        sprintf(packet->payload, "%s", payload);
        packet->size_payload = size_payload;

        packets.push_back(packet);
    }

    if(res == -1) {
        throw std::exception();
    }

    pcap_freecode(&fp);
    pcap_close(handle);
//    packets.clear();
}

PacketInfo::~PacketInfo() {
    delete this->ip_src;
    delete this->ip_dst;
    delete this->dport;
    delete this->sport;
//    delete this->payload;
    delete this->protocol;
    delete this->timestamp;
}

PacketInfo::PacketInfo() {
    this->ip_src = new char[25];
    this->ip_dst = new char[25];
    this->dport = new char[5];
    this->sport = new char[5];
//    this->payload = new char[100];
    this->protocol = new char[10];
    this->timestamp = new char[26];
}

std::string PacketInfo::get_src_ip_info() {
    std::string str = IPSRC_LBL;
    str += this->ip_src;
    return str;
}

std::string PacketInfo::get_dst_ip_info() {
    std::string str = IPDST_LBL;
    str += this->ip_dst;
    return str;
}

std::string PacketInfo::get_src_port_info() {
    std::string str = SRC_PORT_LBL;
    str += this->sport;
    return str;
}

std::string PacketInfo::get_dst_port_info() {
    std::string str = DST_PORT_LBL;
    str += this->dport;
    return str;
}

std::string PacketInfo::get_prtl_info() {
    std::string str = PROTOCOL_LBL;
    str += this->protocol;
    return str;
}

std::string PacketInfo::get_pl_size_info() {
    std::string str = PLSIZE_LBL;
    str += std::to_string(this->size_payload);
    return str;
}

std::string PacketInfo::get_timetamp() {
    auto str = std::string(this->timestamp);
    return str;
}