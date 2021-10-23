#include "pcapworker/pcapworker.hpp"
#define HAVE_REMOTE
#include <pcap.h>
#include <iostream>

void read_device_live(const char* name, int& packet);

int get_devices_amount()
{
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];

    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        return -1;
    }
    /* Print the list */
    for(d=alldevs; d; d=d->next)
    {
        ++i;
    }
    pcap_freealldevs(alldevs);
    return i;
}

void get_devices_info(int& amount, std::vector<device*>& devices) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    device* dev;

    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        return;
    }

    for(d=alldevs; d; d=d->next)
    {
        i++;
        dev = new device();
        dev->number = i;
        dev->name = std::string(d->name);
        if (d->description) dev->description = std::string(d->description);
        devices.push_back(dev);
    }
    amount = i;
}

void read_device_live(const char* name, int& packet, bool& flag) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    pcap_t* adhandle;
    int i = 0;
    int res;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_pkthdr *header;
    const u_char *pkt_data;

    if ((adhandle = pcap_open_live(name,
                                   65536,
                                   1,
                                   1000,
                                   errbuf
                                   )) == NULL)
    {
        throw std::exception();
    }

    while((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 and flag) {
        if (res == 0) {
            continue;
        }
        packet = header->len;
    }

//    if(res == -1) {
//        throw std::exception();
//    }

    pcap_close(adhandle);
}