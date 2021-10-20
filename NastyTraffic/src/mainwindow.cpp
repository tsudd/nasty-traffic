//
// Created by Alex on 10/13/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    traffic = new NastyTraffic();
    for (auto iter = traffic->devices.begin(); iter != traffic->devices.end(); iter++) {
        ui->devicesBox->addItem(QString::fromStdString((*iter)->description));
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete traffic;
}

