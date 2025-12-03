#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
}

AdminMainWindow::~AdminMainWindow() {
    delete ui;
}

void AdminMainWindow::on_flightManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void AdminMainWindow::on_orderViewBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void AdminMainWindow::on_userManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void AdminMainWindow::on_dataStatisticsBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
}

void AdminMainWindow::on_backBtn_clicked() {
    this->close();
}
