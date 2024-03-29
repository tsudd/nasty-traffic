//
// Created by Alex on 10/20/2021.
//

#include <cstring>
#include "nastytraffic.hpp"

void NastyTraffic::get_device_info() {
    devices.clear();
    get_devices_info(device_amount, devices);
}

NastyTraffic::NastyTraffic() {
    get_device_info();
}
NastyTraffic::~NastyTraffic() {
    devices.clear();
    packets.clear();
}

void NastyTraffic::set_device(int number) {
    selected_device = number;
}

void NastyTraffic::read_device(bool& unstop_condition, int port) {
    read_device_live(devices[selected_device]->name.c_str(), packets, unstop_condition, port);
}

void NastyTraffic::clear_packets() {
    packets.clear();
}

PacketInfo* NastyTraffic::get_packet(const int num) {
    if (num < packets.size()) {
        return packets[num];
    }
    return nullptr;
}

int NastyTraffic::get_sniffed_packets_amount() {
    return packets.size();
}

bool NastyTraffic::is_addr_destination(const char *addr) {
    auto ip_net = devices[selected_device]->ip_net;
    if (!addr || !ip_net) {
        return false;
    }
    if (strcmp(addr, ip_net) == 0) {
        return true;
    }
    return false;
}
