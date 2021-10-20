#include "pcapworker/pcapworker.hpp"
#define HAVE_REMOTE
#include <pcap.h>
#include <iostream>

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

void get_devices_info(int& amount, std::list<device*>& devices) {
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