//
// Created by Alex on 11/20/2021.
//

#include "packetorchestrator.hpp"

PacketOrchestrator::PacketOrchestrator(NastyTraffic* traffic_reader) {
    if (!traffic_reader) {
        traffic = new NastyTraffic();
    } else {
        traffic = traffic_reader;
    }
}

PacketOrchestrator::~PacketOrchestrator() {
    delete traffic;
}

void PacketOrchestrator::set_device(const int number) {
    traffic->set_device(number);
}

void PacketOrchestrator::read_device_live(bool &unstop_condition) {
    traffic->read_device(unstop_condition);
}

int PacketOrchestrator::get_sniffed_packets_amount() {
    return traffic->get_sniffed_packets_amount();
}

QList<QString> PacketOrchestrator::get_devices_names() {
    QList<QString> names;
    for (auto iter = traffic->devices.begin(); iter != traffic->devices.end(); iter++) {
        names.push_back(QString::fromStdString((*iter)->description));
    }
    return names;
}

QTreeWidgetItem* PacketOrchestrator::get_packet_item(const int num) {
    auto packet = traffic->get_packet(num);
    auto item = new QTreeWidgetItem(convert_packet_fields(packet->get_timetamp()));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_pl_size_info())));
    return item;
}

QStringList PacketOrchestrator::convert_packet_fields(std::string field) {
    return QStringList(QString::fromStdString(field).split('\n'));
}


