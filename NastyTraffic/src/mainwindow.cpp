//
// Created by Alex on 10/13/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    auto sniffer = new NastyTraffic();
    traffic = new PacketOrchestrator();
    ui->devicesBox->addItems(QStringList(traffic->get_devices_names()));

//    QObject::connect(&ui->devicesBox, &QComboBox::currentIndexChanged, )
    connect((ui->devicesBox), &QComboBox::currentIndexChanged, this, &MainWindow::select_device);
    connect(ui->processButton, &QPushButton::clicked, this, &MainWindow::toggle_process);
    packetModel = ui->treeView->invisibleRootItem();
}

void MainWindow::select_device(int index) {
    traffic->set_device(index);
    if (is_reading) {
        toggle_process();
    }
}

void MainWindow::toggle_process() {
    if (is_reading) {
        is_reading = false;
        ui->processButton->setText("Start");
        ui->statusIndicator->setText("Stopped");

        dumping->join();
        upd->join();

        delete dumping;
        delete upd;
    } else {
        clear();
        is_reading = true;
        ui->processButton->setText("Stop");
        ui->statusIndicator->setText("Sniffing...");
        sniffed_packets = 0;

        auto start = std::chrono::high_resolution_clock::now();

        dumping = new std::thread(&PacketOrchestrator::read_device_live, traffic, std::ref(is_reading));

        upd = new std::thread([&]() {
            while(is_reading) {
//                ui->packagesIndicator->setText(QString::number(packet_len));
            auto tm = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> duration = tm - start;
            show_packets(duration.count());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            start = tm;
            }
        });
    }
}

MainWindow::~MainWindow() {
    is_reading = false;
//    dumping;
//    upd->detach();
//    dumping->join();
//    delete upd;
//    delete dumping;
    delete traffic;
    delete ui;
}

void MainWindow::clear() {
    ui->treeView->clear();
    traffic->clear_packets();
}

void MainWindow::show_packets(const float duration) {
    auto packets_amount = traffic->get_sniffed_packets_amount();
    auto packets = 0;
    long long transfered = 0;
    if (packets_amount == 0) {
        return;
    }
    for (int i = sniffed_packets; i < packets_amount; i++) {
        auto pckt = traffic->get_packet(i);
        packetModel->insertChild(sniffed_packets, traffic->get_packet_item(pckt));
        if (traffic->is_packet_received(pckt)) {
            transfered += traffic->get_packet_size(i);
        }
        packets++;
    }
    ui->packagesIndicator->setText(QString::number(packets));
    ui->seedIndicator->setText(convert_bytes_to_speed(transfered, duration));
    sniffed_packets = packets;
}

QString MainWindow::convert_bytes_to_speed(const long long bytes_amount, const float time) {
    if (time == 0) {
        return "0";
    }
    double speed;
    speed = (bytes_amount / 8.0) / time;

    int division_times = 0;
    do {
        speed /= 1024.0;
        division_times++;
    } while (speed > 1000);
    if (speed > 100) {
        speed *= 1024;
        division_times--;
    }
    auto speed_text = QString::number(round(speed * 10)/10);
    switch (division_times) {
        case 2:
            return speed_text + " MB/s";
        default:
            return  speed_text + " KB/s";
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, QString(APP_NAME),
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        is_reading = false;
//        if (dumping)
//            dumping->detach();
//        if (upd)
//            upd->detach();
        event->accept();
    }
}


