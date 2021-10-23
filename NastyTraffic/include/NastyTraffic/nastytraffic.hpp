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
    std::vector<device*> devices;
    NastyTraffic();
    ~NastyTraffic();
    int get_device_amount();
    void get_device_info();
    void read_device(int& info, bool& unstop_condition);
    void set_device(int number);

private:
    int selected_device = 0;
};


#endif //NASTYTRAFFIC_NASTYTRAFFIC_H
