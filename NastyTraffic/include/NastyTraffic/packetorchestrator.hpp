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
    void read_device_live(bool& unstop_condition);
    int get_sniffed_packets_amount();
    QTreeWidgetItem* get_packet_item(const int num);
    void clear_data();
    QList<QString> get_devices_names();
private:
    NastyTraffic* traffic;
    QStringList convert_packet_fields(std::string field);
};
#endif //NASTYTRAFFIC_PACKETORCHESTRATOR_H
