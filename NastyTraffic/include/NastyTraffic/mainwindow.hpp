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
#include <QGraphicsScene>
#include <QLineSeries>
#include "qcustomplot.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
    struct  point {
        double key;
        double value;
    };
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void select_device(int index);
    void toggle_process();
    void graph_update_tick();
    void clear_graphs();
private:
    std::thread* dumping;
    std::thread* upd;

    QTimer* dataTimer;

    QCustomPlot* transferPlt;
    QCustomPlot* pcktPlt;
    QQueue<double> downPnts;
    QQueue<double> upPnts;
    QQueue<double> pcktPnts;
    QQueue<double> keys;

    double time_passed = 0;
    int sniffed_packets = 0;
    bool is_reading = false;
    PacketOrchestrator* traffic;
    Ui::MainWindow *ui;
    void addPoint(const double down, const double up, const double packets);
    void closeEvent(QCloseEvent *event) override;
    void clear();

    QTreeWidgetItem* packetModel;

    void show_packets(const double duration);
    QString convert_bytes_to_speed(const long long bytes_amount, const float time);
};


#endif //SNIFFER_MAINWINDOW_H
