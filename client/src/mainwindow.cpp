#include "client/mainwindow.hpp"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *label = new QLabel("Panorama Viewer", this);
    setCentralWidget(label);
}
