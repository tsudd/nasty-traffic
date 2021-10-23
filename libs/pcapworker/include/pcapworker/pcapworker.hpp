#pragma once
#include <string>
#include <vector>

struct device {
    std::string name;
    int number;
    std::string description;
    device* next;
};

int get_devices_amount();
void get_devices_info(int& amount, std::vector<device*>& devices);
void read_device_live(const char* name, int& packet, bool& flag);
