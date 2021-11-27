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

void PacketOrchestrator::read_device_live(bool &unstop_condition, const int port) {
    traffic->read_device(unstop_condition, port);
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

const PacketInfo* PacketOrchestrator::get_packet(const int num) const {
    return traffic->get_packet(num);
}

QTreeWidgetItem* PacketOrchestrator::get_packet_item(const int num) const {
    auto packet = traffic->get_packet(num);
    auto item = new QTreeWidgetItem(convert_packet_fields(packet->get_timetamp()));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_prtl_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_pl_size_info())));
    return item;
}

QTreeWidgetItem* PacketOrchestrator::get_packet_item(const PacketInfo *packet) const {
    auto item = new QTreeWidgetItem(convert_packet_fields(packet->get_timetamp()));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_ip_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_prtl_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_src_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_dst_port_info())));
    item->addChild(new QTreeWidgetItem(convert_packet_fields(packet->get_pl_size_info())));
    return item;
}

QStringList PacketOrchestrator::convert_packet_fields(std::string field) const {
    return QStringList(QString::fromStdString(field).split('\n'));
}

int PacketOrchestrator::get_packet_size(const int num) {
    return traffic->get_packet(num)->size_payload;
}

bool PacketOrchestrator::is_packet_received(const PacketInfo *packet) {
    return traffic->is_addr_destination(packet->ip_dst);
}

void PacketOrchestrator::clear_packets() {
    traffic->clear_packets();
}


