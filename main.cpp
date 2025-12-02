// #include "UserMainWindow.h"
// #include "LoginDlg.h"
#include "DBOperator.h"
#include "WindowFactory.h"

#include <QApplication>
#include <QDialog>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // qDebug() << "Starting database test...";

    // 创建数据库操作对象
    DBOperator db;
    // 连接数据库
    db.DBOpen();

    // LoginDlg *loginDlg = new LoginDlg;
    // UserMainWindow *mainWin = nullptr; // 主窗口指针

    // while (true) {
    //     if (loginDlg->exec() == QDialog::Accepted) {
    //         // 登录成功：创建并显示主窗口
    //         mainWin = new UserMainWindow(loginDlg, nullptr);
    //         mainWin->setAttribute(Qt::WA_DeleteOnClose); // 主窗口关闭时自动释放
    //         mainWin->show();

    //         // 等待主窗口关闭 (阻塞到主窗口销毁)
    //         a.exec();
    //         mainWin = nullptr; // 主窗口关闭后置空

    //         loginDlg->clearInput(); // 调用公共接口清空输入
    //     } else {
    //         // 登录取消：退出循环并释放资源
    //         delete loginDlg;
    //         break;
    //     }
    // }

    // 设置应用程序信息
    a.setApplicationName("StudentGradeSystem");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("YourSchool");

    qDebug() << "应用程序启动...";

    // 获取窗口工厂实例
    WindowFactory &factory = WindowFactory::instance();

    // 创建并显示登录窗口
    QDialog *loginWindow = factory.createLoginWindow();
    if (loginWindow) {
        loginWindow->show();

        // 连接注销请求（当主窗口注销时，回到登录界面）
        QObject::connect(&factory, &WindowFactory::logoutRequested,
                         [loginWindow, &factory]() {
                             // 如果有登录窗口，先删除
                             if (loginWindow) {
                                 loginWindow->deleteLater();
                             }

                             // 创建新的登录窗口
                             QDialog *newLoginWindow = factory.createLoginWindow();
                             if (newLoginWindow) {
                                 newLoginWindow->show();

                                 // 更新loginWindow指针（如果需要）
                                 // 注意：这里lambda捕获的loginWindow是局部变量的引用
                                 // 实际使用中可能需要更好的管理方式
                             }
                         });
    } else {
        QMessageBox::critical(nullptr, "错误", "无法创建登录窗口！");
        return -1;
    }

    // 连接应用程序退出信号
    QObject::connect(&a, &QApplication::aboutToQuit, []() {
        qDebug() << "应用程序即将退出...";
        // 执行清理操作
    });

    return a.exec();
}
