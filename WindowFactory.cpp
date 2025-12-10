#include "WindowFactory.h"

#include "AdminMainWindow.h"
#include "LoginDlg.h"
#include "UserMainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>

// 获取单例实例
WindowFactory &WindowFactory::instance() {
    static WindowFactory instance;
    return instance;
}

// 构造函数：初始化窗口工厂
WindowFactory::WindowFactory(QObject *parent) : QObject(parent), m_currentMainWindow(nullptr) {}

// 创建主窗口（用户或管理员）
QMainWindow *WindowFactory::createMainWindow(bool isAdmin, int userId, QWidget *parent) {
    destroyCurrentWindow();

    QMainWindow *window = nullptr;

    // 根据用户类型创建对应的主窗口
    if (!isAdmin) {
        window = new UserMainWindow(userId);
        window->setWindowTitle("用户界面");
    } else {
        window = new AdminMainWindow(parent);
        window->setWindowTitle("管理员界面");
    }

    if (window) {
        m_currentMainWindow = window;

        // 连接注销信号
        if (!isAdmin) {
            UserMainWindow *userWindow = qobject_cast<UserMainWindow *>(window);
            if (userWindow) {
                connect(userWindow, &UserMainWindow::logoutRequested, this, &WindowFactory::switchToLogin);
            }
        } else {
            AdminMainWindow *adminWindow = qobject_cast<AdminMainWindow *>(window);
            if (adminWindow) {
                connect(adminWindow, &AdminMainWindow::logoutRequested, this, &WindowFactory::switchToLogin);
            }
        }

        // 窗口销毁时清空指针
        connect(window, &QMainWindow::destroyed, this, [this]() {
            m_currentMainWindow = nullptr;
        });

        emit mainWindowCreated(window);
    }

    return window;
}

// 创建登录窗口
QDialog *WindowFactory::createLoginWindow(QWidget *parent) {
    LoginDlg *loginDlg = new LoginDlg(parent);
    loginDlg->setWindowTitle("用户登录");

    // 连接登录成功信号，创建对应的主窗口
    connect(loginDlg, &LoginDlg::loginSuccess, this,
            [this, loginDlg](int userId, const QString &username, bool isAdmin) {
                Q_UNUSED(username);
                QMainWindow *mainWindow = createMainWindow(isAdmin, userId);
                if (mainWindow) {
                    // 居中显示主窗口
                    mainWindow->adjustSize();
                    QScreen *screen = QApplication::primaryScreen();
                    QRect screenGeometry = screen->geometry();
                    QRect windowGeometry = mainWindow->geometry();
                    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
                    int y = (screenGeometry.height() - windowGeometry.height()) / 2 - windowGeometry.height() / 10;
                    mainWindow->move(x, y);
                    mainWindow->show();
                }
                loginDlg->deleteLater();
            });

    // 连接取消登录信号，退出程序
    connect(loginDlg, &LoginDlg::rejected, this, []() {
        QApplication::quit();
    });

    return loginDlg;
}

// 切换到登录窗口 (用于注销)
void WindowFactory::switchToLogin() {
    destroyCurrentWindow();
    emit logoutRequested();

    QDialog *loginDlg = createLoginWindow();
    loginDlg->show();
}

QMainWindow *WindowFactory::currentMainWindow() const {
    return m_currentMainWindow;
}

void WindowFactory::destroyCurrentWindow() {
    if (m_currentMainWindow) {
        m_currentMainWindow->close();
        m_currentMainWindow->deleteLater();
        m_currentMainWindow = nullptr;
    }
}
