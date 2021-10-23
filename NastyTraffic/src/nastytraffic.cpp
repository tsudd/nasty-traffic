//
// Created by Alex on 10/20/2021.
//

#include "nastytraffic.hpp"

int NastyTraffic::get_device_amount() {
    return get_devices_amount();
}

void NastyTraffic::get_device_info() {
    get_devices_info(device_amount, devices);
}

NastyTraffic::NastyTraffic() {
    get_device_info();
}
NastyTraffic::~NastyTraffic() {
    devices.clear();
}

void NastyTraffic::set_device(int number) {
    selected_device = number;
}

void NastyTraffic::read_device(int &info, bool& unstop_condition) {
    read_device_live(devices[selected_device]->name.c_str(), info, unstop_condition);
}