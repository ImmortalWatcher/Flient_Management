#include "DBOperator.h"
#include "WindowFactory.h"

#include <QApplication>
#include <QDialog>
#include <QMessageBox>

// 程序入口函数
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // 打开数据库连接
    DBOperator db;
    db.DBOpen();

    // 设置应用程序信息
    a.setApplicationName("FlightManagementSystem");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("FlightManagement");

    // 创建登录窗口
    WindowFactory &factory = WindowFactory::instance();
    QDialog *loginWindow = factory.createLoginWindow();
    if (loginWindow) {
        loginWindow->show();
    } else {
        QMessageBox::critical(nullptr, "错误", "无法创建登录窗口！");
        return -1;
    }

    return a.exec();
}
