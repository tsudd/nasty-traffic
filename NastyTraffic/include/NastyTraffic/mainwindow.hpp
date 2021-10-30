//
// Created by Alex on 10/13/2021.
//

#ifndef SNIFFER_MAINWINDOW_H
#define SNIFFER_MAINWINDOW_H

#include <QWidget>
#include <QString>
#include "nastytraffic.hpp"
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void selectDevice(int index);
    void toggleProcess();
private:
    std::thread* dumping;
    std::thread* upd;
    int packet_len;
    bool is_reading = false;
    NastyTraffic* traffic;
    Ui::MainWindow *ui;
};


#endif //SNIFFER_MAINWINDOW_H
