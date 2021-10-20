//
// Created by Alex on 10/13/2021.
//

#ifndef SNIFFER_MAINWINDOW_H
#define SNIFFER_MAINWINDOW_H

#include <QWidget>
#include <QString>
#include "nastytraffic.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    NastyTraffic* traffic;
    Ui::MainWindow *ui;
};


#endif //SNIFFER_MAINWINDOW_H
