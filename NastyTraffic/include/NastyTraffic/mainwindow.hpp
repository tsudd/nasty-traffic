//
// Created by Alex on 10/13/2021.
//

#ifndef SNIFFER_MAINWINDOW_H
#define SNIFFER_MAINWINDOW_H

#define APP_NAME "NastyTraffic"
#include <QWidget>
#include <QString>
#include <QTreeWidgetItem>
#include "packetorchestrator.hpp"
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
    void select_device(int index);
    void toggle_process();
    void clear_widgets();
private:
    std::thread* dumping;
    std::thread* upd;
    int sniffed_packets = 0;
    bool is_reading = false;
    PacketOrchestrator* traffic;
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event) override;

    QTreeWidgetItem* packetModel;

    void show_packets();
};


#endif //SNIFFER_MAINWINDOW_H
