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

    LoginDlg *loginDlg = new LoginDlg;
    MainWindow *mainWin = nullptr; // 主窗口指针

    while (true) {
        if (loginDlg->exec() == QDialog::Accepted) {
            // 登录成功：创建并显示主窗口
            mainWin = new MainWindow(loginDlg, nullptr);
            mainWin->setAttribute(Qt::WA_DeleteOnClose); // 主窗口关闭时自动释放
            mainWin->show();

            // 等待主窗口关闭 (阻塞到主窗口销毁)
            a.exec();
            mainWin = nullptr; // 主窗口关闭后置空

            loginDlg->clearInput(); // 调用公共接口清空输入
        } else {
            // 登录取消：退出循环并释放资源
            delete loginDlg;
            break;
        }
    }

    return 0;
}
