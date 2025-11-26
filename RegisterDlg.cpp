#include "RegisterDlg.h"
#include "ui_RegisterDlg.h"

RegDlg::RegDlg(QWidget *parent) : QDialog(parent), ui(new Ui::RegDlg) {
    ui->setupUi(this);
}

RegDlg::~RegDlg() {
    delete ui;
}
