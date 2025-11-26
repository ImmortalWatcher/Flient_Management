#include "mainwindow.h"
#include "LoginDlg.h"
#include "DBOperator.h"

#include <QApplication>
#include <QDialog>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // qDebug() << "Starting database test...";

    // 创建数据库操作对象
    DBOperator db;

    // 连接数据库
    db.DBOpen();

    LoginDlg loginDlg;
    if (loginDlg.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }
    else return 0;
}
