//
// Created by Alex on 10/13/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved
#define POINTS_AMOUNT 20

#include "mainwindow.hpp"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <cmath>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    traffic = new PacketOrchestrator();
    dataTimer = new QTimer();
    ui->devicesBox->addItems(QStringList(traffic->get_devices_names()));

    //graphs set up
    transferPlt = new QCustomPlot();
    ui->graphGrid->addWidget(transferPlt, 0, 0, 1, 1);

    transferPlt->setInteraction(QCP::iRangeDrag, true);
    transferPlt->axisRect()->setRangeDrag(Qt::Horizontal);
    transferPlt->xAxis->setSubTicks(true);

    transferPlt->xAxis2->setVisible(true);
    transferPlt->yAxis2->setVisible(true);
    transferPlt->xAxis2->setTicks(false);
    transferPlt->yAxis2->setTicks(false);
    transferPlt->xAxis2->setTickLabels(false);
    transferPlt->yAxis2->setTickLabels(false);

    transferPlt->yAxis->setTickLabelColor(QColor(Qt::red));
    transferPlt->legend->setVisible(true);
    transferPlt->yAxis->setRange(0, 13000);

    transferPlt->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    transferPlt->addGraph();
    transferPlt->graph(0)->setName("Download, KB/s");
    QPen pen1 = QPen(QColor(40, 110, 255));
    pen1.setWidth(3);
    transferPlt->graph(0)->setPen(pen1);
    transferPlt->graph(0)->setAntialiased(false);
    transferPlt->graph(0)->setLineStyle(QCPGraph::lsLine);

    transferPlt->addGraph();
    transferPlt->graph(1)->setName("Upload, KB/s");
    QPen pen2 = QPen(QColor(255,0,0));
    pen2.setWidth(3);
    transferPlt->graph(1)->setPen(QPen(pen2));
    transferPlt->graph(1)->setAntialiased(false);
    transferPlt->graph(1)->setLineStyle(QCPGraph::lsLine);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s");
    transferPlt->yAxis->setRange(0, 15000);
    transferPlt->xAxis->setTicker(timeTicker);
    transferPlt->axisRect()->setupFullAxesBox();

    connect(transferPlt->xAxis, SIGNAL(rangeChanged(QCPRange)),
            transferPlt->xAxis2, SLOT(setRange(QCPRange)));

    connect(transferPlt->yAxis, SIGNAL(rangeChanged(QCPRange)),
            transferPlt->yAxis2, SLOT(setRange(QCPRange)));

    connect(dataTimer, SIGNAL(timeout()), this, SLOT(graph_update_tick()));

    pcktPlt = new QCustomPlot();
    ui->graphGrid->addWidget(pcktPlt, 1, 0, 1, 1);

    pcktPlt->setInteraction(QCP::iRangeDrag, true);
    pcktPlt->axisRect()->setRangeDrag(Qt::Horizontal);
    pcktPlt->xAxis->setSubTicks(true);

    pcktPlt->xAxis2->setVisible(true);
    pcktPlt->yAxis2->setVisible(true);
    pcktPlt->xAxis2->setTicks(false);
    pcktPlt->yAxis2->setTicks(false);
    pcktPlt->xAxis2->setTickLabels(false);
    pcktPlt->yAxis2->setTickLabels(false);

    pcktPlt->yAxis->setTickLabelColor(QColor(Qt::red));
    pcktPlt->legend->setVisible(true);
    pcktPlt->yAxis->setRange(0, 13000);

    pcktPlt->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    pcktPlt->addGraph();
    pcktPlt->graph(0)->setName("Packets amount, N");
    QPen pen3 = QPen(QColor(40, 110, 255));
    pen3.setWidth(3);
    pcktPlt->graph(0)->setPen(pen3);
    pcktPlt->graph(0)->setAntialiased(false);
    pcktPlt->graph(0)->setLineStyle(QCPGraph::lsLine);

    pcktPlt->yAxis->setRange(0, 400);
    pcktPlt->xAxis->setTicker(timeTicker);
    pcktPlt->axisRect()->setupFullAxesBox();

    connect(pcktPlt->xAxis, SIGNAL(rangeChanged(QCPRange)),
            pcktPlt->xAxis2, SLOT(setRange(QCPRange)));

    connect(pcktPlt->yAxis, SIGNAL(rangeChanged(QCPRange)),
            pcktPlt->yAxis2, SLOT(setRange(QCPRange)));

    connect((ui->devicesBox), SIGNAL(currentIndexChanged(int)), this, SLOT(select_device(int)));
    connect(ui->processButton, SIGNAL(clicked()), this, SLOT(toggle_process()));
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

        dumping->detach();
        upd->detach();
        dataTimer->stop();
        delete dumping;
        delete upd;
    } else {
        clear();
        is_reading = true;
        dataTimer->start(0);
        ui->processButton->setText("Stop");
        ui->statusIndicator->setText("Sniffing...");
        sniffed_packets = 0;
        clear_graphs();

        dumping = new std::thread(&PacketOrchestrator::read_device_live, traffic, std::ref(is_reading));

        upd = new std::thread([&]() {
            auto start = std::chrono::steady_clock::now();
            do {
            auto tm = std::chrono::steady_clock::now();
            show_packets(std::chrono::duration_cast<std::chrono::seconds>(tm-start).count());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            start = tm;
            } while(is_reading);
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
    delete dataTimer;
}

void MainWindow::clear() {
    ui->treeView->clear();
    traffic->clear_packets();
}

void MainWindow::show_packets(const double duration) {
    time_passed += duration;
    int amount = traffic->get_sniffed_packets_amount();
    if (amount - sniffed_packets == 0) {
        addPoint(0, 0, 0);
        return;
    }
    int packets = 0;
    long long downloaded = 0;
    long long uploaded = 0;
    for (int i = sniffed_packets; i < amount; i++) {
        auto pckt = traffic->get_packet(i);
        packetModel->insertChild(sniffed_packets, traffic->get_packet_item(pckt));
        if (traffic->is_packet_received(pckt)) {
            downloaded += pckt->size_payload / 8;
        } else {
            uploaded += pckt->size_payload / 8;
        }
        packets++;
    }
    addPoint(downloaded / 1024.0 / duration, uploaded / 1024.0 / duration, packets);
    ui->packagesIndicator->setText(QString::number(packets));
    ui->downloadIndicator->setText(convert_bytes_to_speed(downloaded, duration));
    ui->uploadIndicator->setText(convert_bytes_to_speed(uploaded, duration));
    sniffed_packets += packets;
}

QString MainWindow::convert_bytes_to_speed(const long long bytes_amount, const float time) {
    if (time == 0) {
        return "0";
    }
    double speed;
    speed = bytes_amount / time;

    int division_times = 0;
    do {
        speed /= 1024.0;
        division_times++;
    } while (speed > 1000);
//    if (speed > 100) {
//        speed *= 1024;
//        division_times--;
//    }
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

void MainWindow::addPoint(const double down, const double up, const double packets) {
    downPnts.dequeue();
    keys.dequeue();
    upPnts.dequeue();
    pcktPnts.dequeue();
    upPnts.enqueue(up);
    pcktPnts.enqueue(packets);
    downPnts.enqueue(down);
    keys.enqueue(time_passed);
}

void MainWindow::graph_update_tick() {
    if (keys.isEmpty()) {
        return;
    }
    auto moments = keys.toVector();
    transferPlt->graph(0)->setData(moments, downPnts.toVector());
    transferPlt->graph(1)->setData(moments, upPnts.toVector());
    pcktPlt->graph(0)->setData(moments, pcktPnts.toVector());
    pcktPlt->xAxis->setRange(moments[moments.size() - 1], 8, Qt::AlignRight);
    transferPlt->xAxis->setRange(time_passed, 8, Qt::AlignRight);
    pcktPlt->replot();
    transferPlt->replot();
}

void MainWindow::clear_graphs() {
    keys.clear();
    downPnts.clear();
    upPnts.clear();
    pcktPnts.clear();

    for (int i = 0; i < POINTS_AMOUNT; i++) {
        downPnts.enqueue(0);
        pcktPnts.enqueue(0);
        upPnts.enqueue(0);
        keys.enqueue(0);
    }
}


