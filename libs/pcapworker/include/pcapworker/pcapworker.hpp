#pragma once
#include <string>
#include <vector>

struct Device {
    std::string name;
    int number;
    char* ip_net = NULL;
    std::string description;
    Device* next;

//    ~Device();
};

struct PacketInfo {
    int number = 0;
    char* ip_src;
    char* ip_dst;
    char* sport;
    char* dport;
    char* protocol;
//    char* payload;
    char* timestamp;
    int size_payload = 0;

    PacketInfo();
    ~PacketInfo();
    std::string get_src_ip_info() const;
    std::string get_dst_ip_info() const;
    std::string get_src_port_info() const;
    std::string get_dst_port_info() const;
    std::string get_prtl_info() const;
//    std::string get_pl_info();
    std::string get_pl_size_info() const;
    std::string get_timetamp() const;
};

void get_devices_info(int& amount, std::vector<Device*>& devices);
void read_device_live(const char* dev, std::vector<PacketInfo*>& packets, const bool& flag, int port);
