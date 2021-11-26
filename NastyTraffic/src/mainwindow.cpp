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

    downPlt = new QCustomPlot();
    ui->graphGrid->addWidget(downPlt, 0, 0, 1, 1);


//    downPlt->setInteraction(QCP::iRangeZoom, true);
    downPlt->setInteraction(QCP::iRangeDrag, true);
    downPlt->axisRect()->setRangeDrag(Qt::Horizontal);
//    downPlt->axisRect()->setRangeZoom(Qt::Horizontal);
    downPlt->xAxis->setSubTicks(true);

    downPlt->xAxis2->setVisible(true);
    downPlt->yAxis2->setVisible(true);
    downPlt->xAxis2->setTicks(false);
    downPlt->yAxis2->setTicks(false);
    downPlt->xAxis2->setTickLabels(false);
    downPlt->yAxis2->setTickLabels(false);

    downPlt->yAxis->setTickLabelColor(QColor(Qt::red));
    downPlt->legend->setVisible(true);
    downPlt->yAxis->setRange(0, 13000);

    downPlt->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

//    downGraph = new QCPGraph(downPlt->xAxis, downPlt->yAxis);
//    downPlt->addPlttable
    downPlt->addGraph();
    downPlt->graph(0)->setName("Download, B/s");
    downPlt->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    downPlt->graph(0)->setAntialiased(false);
    downPlt->graph(0)->setLineStyle(QCPGraph::lsLine);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s");
    downPlt->yAxis->setRange(0, 20000);
    downPlt->xAxis->setTicker(timeTicker);
    downPlt->axisRect()->setupFullAxesBox();

    connect(downPlt->xAxis, SIGNAL(rangeChanged(QCPRange)),
            downPlt->xAxis2, SLOT(setRange(QCPRange)));

    connect(downPlt->yAxis, SIGNAL(rangeChanged(QCPRange)),
            downPlt->yAxis2, SLOT(setRange(QCPRange)));

    connect(dataTimer, SIGNAL(timeout()), this, SLOT(download_range_changed()));
//    downPlt->graph(0)->setData(time, downPoints);

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
        addPoint(0);
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
            uploaded += pckt->size_payload;
        }
        packets++;
    }
    addPoint(downloaded / 1024.0 / duration);
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

void MainWindow::addPoint(const double p) {
    downPnts.dequeue();
    keys.dequeue();
    downPnts.enqueue(p);
    keys.enqueue(time_passed);
}

void MainWindow::download_range_changed() {
    if (downPnts.isEmpty() || keys.isEmpty()) {
        return;
    }
    downPlt->graph(0)->setData(keys.toVector(), downPnts.toVector());
//    downPlt->graph(0)->rescaleValueAxis();
    downPlt->xAxis->setRange(time_passed, 8, Qt::AlignRight);
    downPlt->replot();
}

void MainWindow::clear_graphs() {
    keys.clear();
    downPnts.clear();

    for (int i = 0; i < POINTS_AMOUNT; i++) {
        downPnts.enqueue(0);
        keys.enqueue(0);
    }
}


