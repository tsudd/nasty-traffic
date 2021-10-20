#pragma once
#include <string>
#include <list>

struct device {
    std::string name;
    int number;
    std::string description;
    device* next;
};

int get_devices_amount();
void get_devices_info(int& amount, std::list<device*>& devices);
