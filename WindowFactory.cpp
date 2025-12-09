#include "WindowFactory.h"
#include "LoginDlg.h"
#include "UserMainWindow.h"
#include "AdminMainWindow.h"

#include <QApplication>
#include <QScreen>
#include <QDebug>

WindowFactory& WindowFactory::instance() {
    static WindowFactory instance;
    return instance;
}

WindowFactory::WindowFactory(QObject *parent) : QObject(parent), m_currentMainWindow(nullptr) {}

QMainWindow* WindowFactory::createMainWindow(bool isAdmin, int userId, QWidget *parent) {
    // 如果已有窗口，先销毁
    destroyCurrentWindow();

    QMainWindow *window = nullptr;

    if (!isAdmin) { // 普通用户
        window = new UserMainWindow(userId);
        window->setWindowTitle("用户界面");
        qDebug() << "创建普通用户窗口，用户ID:" << userId;
    } else {        // 管理员
        window = new AdminMainWindow(parent);
        window->setWindowTitle("管理员界面");
        qDebug() << "创建管理员窗口";
    }

    if (window) {
        m_currentMainWindow = window;

        // 连接窗口关闭信号
        connect(window, &QMainWindow::destroyed, this, [this]() {
            m_currentMainWindow = nullptr;
            qDebug() << "主窗口已销毁";
        });

        emit mainWindowCreated(window);
    }

    return window;
}

QDialog* WindowFactory::createLoginWindow(QWidget *parent) {
    LoginDlg *loginDlg = new LoginDlg(parent);
    loginDlg->setWindowTitle("用户登录");
    
    // 居中显示登录窗口
    // loginDlg->adjustSize();
    // QScreen *screen = QApplication::primaryScreen();
    // QRect screenGeometry = screen->geometry();
    // QRect dialogGeometry = loginDlg->geometry();
    // int x = (screenGeometry.width() - dialogGeometry.width()) / 2;
    // int y = (screenGeometry.height() - dialogGeometry.height()) / 2;
    // loginDlg->move(x, y);

    // 连接登录成功信号
    connect(loginDlg, &LoginDlg::loginSuccess, this,
            [this, loginDlg](int userId, const QString &username, bool isAdmin) {
                qDebug() << "登录成功，用户ID:" << userId << "用户名:" << username << "是否管理员:" << isAdmin;

                // 创建对应的主窗口，传递用户 ID
                QMainWindow *mainWindow = createMainWindow(isAdmin, userId);
                if (mainWindow) {
                    // 居中显示窗口
                    mainWindow->adjustSize();
                    QScreen *screen = QApplication::primaryScreen();
                    QRect screenGeometry = screen->geometry();
                    QRect windowGeometry = mainWindow->geometry();
                    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
                    int y = (screenGeometry.height() - windowGeometry.height()) / 2 - windowGeometry.height() / 10;
                    mainWindow->move(x, y);
                    mainWindow->show();
                }

                // 删除登录窗口
                loginDlg->deleteLater();
            });

    // 连接登录取消信号
    connect(loginDlg, &LoginDlg::rejected, this, []() {
        qDebug() << "登录取消，退出程序";
        QApplication::quit();
    });

    return loginDlg;
}

void WindowFactory::switchToLogin() {
    // 销毁当前主窗口
    destroyCurrentWindow();

    // 发射注销信号
    emit logoutRequested();

    // 创建并显示登录窗口
    QDialog *loginDlg = createLoginWindow();
    loginDlg->show();
}

QMainWindow* WindowFactory::currentMainWindow() const {
    return m_currentMainWindow;
}

void WindowFactory::destroyCurrentWindow() {
    if (m_currentMainWindow) {
        m_currentMainWindow->close();
        m_currentMainWindow->deleteLater();
        m_currentMainWindow = nullptr;
    }
}
