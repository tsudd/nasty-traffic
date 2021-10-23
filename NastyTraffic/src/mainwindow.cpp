//
// Created by Alex on 10/13/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"
#include "ui_MainWindow.h"

#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    traffic = new NastyTraffic();
    for (auto iter = traffic->devices.begin(); iter != traffic->devices.end(); iter++) {
        ui->devicesBox->addItem(QString::fromStdString((*iter)->description));
    }

//    QObject::connect(&ui->devicesBox, &QComboBox::currentIndexChanged, )
    connect((ui->devicesBox), &QComboBox::currentIndexChanged, this, &MainWindow::selectDevice);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startReading);
    connect(ui->endButton, &QPushButton::clicked, this, &MainWindow::endReading);
//    connect(ui->packetsLabel, &QLabel::)

    ui->endButton->setDisabled(true);
    ui->packetsLabel->setText("0");
}

void MainWindow::selectDevice(int index) {
    traffic->set_device(index);
}

void MainWindow::startReading() {
    is_reading = true;
    packet_len = 0;

    ui->endButton->setDisabled(false);
    ui->startButton->setDisabled(true);

    dumping = new std::thread(&NastyTraffic::read_device, traffic, std::ref(packet_len), std::ref(is_reading));

    upd = new std::thread([&]() {
        while(is_reading) {
            ui->packetsLabel->setText(QString::number(packet_len));
//            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void MainWindow::endReading() {
    is_reading = false;
    ui->endButton->setDisabled(true);
    ui->startButton->setDisabled(false);

    ui->packetsLabel->setText(QString::number(0));

    dumping->detach();
    upd->detach();

    delete dumping;
    delete upd;
}


MainWindow::~MainWindow() {
    delete ui;
    delete traffic;
    dumping->detach();
    upd->detach();
    delete upd;
    delete dumping;
}

