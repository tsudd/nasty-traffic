//
// Created by Alex on 10/13/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"
#include "ui_MainWindow.h"

#include <chrono>
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

        dumping->join();
        upd->join();

        delete dumping;
        delete upd;
    } else {
        is_reading = true;
        ui->processButton->setText("Stop");
        sniffed_packets = 0;
        clear_widgets();

        dumping = new std::thread(&PacketOrchestrator::read_device_live, traffic, std::ref(is_reading));

        upd = new std::thread([&]() {
            while(is_reading) {
//                ui->packagesIndicator->setText(QString::number(packet_len));
            show_packets();
            std::this_thread::sleep_for(std::chrono::seconds(1));
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

void MainWindow::clear_widgets() {
    ui->treeView->clearMask();
}

void MainWindow::show_packets() {
    auto packets_amount = traffic->get_sniffed_packets_amount();
    auto packets = 0;
    if (packets_amount == 0) {
        return;
    }
    for (int i = sniffed_packets; i < packets_amount; i++) {
        packetModel->insertChild(sniffed_packets, traffic->get_packet_item(i));
        packets++;
    }
    sniffed_packets = packets;
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


