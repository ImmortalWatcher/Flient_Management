#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminMainWindow)
{
    ui->setupUi(this);
}

AdminMainWindow::~AdminMainWindow()
{
    delete ui;
}
