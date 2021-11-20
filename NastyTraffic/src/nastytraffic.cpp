//
// Created by Alex on 10/20/2021.
//

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

void NastyTraffic::read_device(bool& unstop_condition) {
    read_device_live(devices[selected_device]->name.c_str(), packets, unstop_condition, -1);
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
