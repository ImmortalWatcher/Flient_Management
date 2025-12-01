#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(LoginDlg *loginDlg, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_loginDlg(loginDlg) {
    ui->setupUi(this);
    connect(ui->backBtn, &QPushButton::clicked, this, &MainWindow::on_backBtn_clicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_flightQueryBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_myOrdersBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_myFavoritesBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_personalCenterBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_backBtn_clicked() {
    this->close();
}
