//
// Created by Alex on 11/20/2021.
//

#ifndef NASTYTRAFFIC_PACKETORCHESTRATOR_H
#define NASTYTRAFFIC_PACKETORCHESTRATOR_H
#include "nastytraffic.hpp"
#include <QTreeWidgetItem>
class PacketOrchestrator {
public:
    explicit PacketOrchestrator(NastyTraffic* traffic_reader = nullptr);
    ~PacketOrchestrator();
    void set_device(const int number);
    void read_device_live(bool& unstop_condition, const int port);
    int get_sniffed_packets_amount();
    const PacketInfo* get_packet(const int num) const;
    bool is_packet_received(const PacketInfo* packet);
    QTreeWidgetItem* get_packet_item(const int num) const;
    QTreeWidgetItem* get_packet_item(const PacketInfo* packet) const;
    void clear_packets();
    QList<QString> get_devices_names();
    int get_packet_size(const int num);
private:
    NastyTraffic* traffic;
    QStringList convert_packet_fields(std::string field) const;
};
#endif //NASTYTRAFFIC_PACKETORCHESTRATOR_H
