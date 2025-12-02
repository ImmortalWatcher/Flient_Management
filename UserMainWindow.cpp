#include "UserMainWindow.h"
#include "ui_UserMainWindow.h"

UserMainWindow::UserMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);
}

UserMainWindow::~UserMainWindow() { delete ui; }

void UserMainWindow::on_flightQueryBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void UserMainWindow::on_myOrdersBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void UserMainWindow::on_myFavoritesBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void UserMainWindow::on_personalCenterBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
}

void UserMainWindow::on_backBtn_clicked() {
    this->close();
}
