//
// Created by Alex on 10/20/2021.
//

#ifndef NASTYTRAFFIC_NASTYTRAFFIC_H
#define NASTYTRAFFIC_NASTYTRAFFIC_H
#include <vector>
#include "pcapworker/pcapworker.hpp"

class NastyTraffic {
public:
    int device_amount = 0;
    std::vector<Device*> devices;
    NastyTraffic();
    ~NastyTraffic();
    void get_device_info();
    void read_device(bool& unstop_condition, int port);
    void set_device(int number);
    bool is_addr_destination(const char* addr);
    PacketInfo* get_packet(const int num);
    int get_sniffed_packets_amount();
    void clear_packets();
private:
    int selected_device = 0;
    std::vector<PacketInfo*> packets = std::vector<PacketInfo*>();
};


#endif //NASTYTRAFFIC_NASTYTRAFFIC_H
