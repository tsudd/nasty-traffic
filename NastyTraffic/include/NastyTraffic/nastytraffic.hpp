//
// Created by Alex on 10/20/2021.
//

#ifndef NASTYTRAFFIC_NASTYTRAFFIC_H
#define NASTYTRAFFIC_NASTYTRAFFIC_H
#include <list>
#include "pcapworker/pcapworker.hpp"

class NastyTraffic {
public:
    int device_amount = 0;
    std::list<device*> devices;
    NastyTraffic();
    ~NastyTraffic();
    int get_device_amount();
    void get_device_info();
};


#endif //NASTYTRAFFIC_NASTYTRAFFIC_H
