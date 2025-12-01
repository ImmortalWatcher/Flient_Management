#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(LoginDlg *loginDlg,QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),m_loginDlg(loginDlg){
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_backBtn_clicked()
{
    this->close();
    if(m_loginDlg){
        m_loginDlg->show();
    }
}

