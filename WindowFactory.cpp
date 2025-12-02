#include "WindowFactory.h"
#include "LoginDlg.h"
#include "AdminMainWindow.h"
#include "UserMainWindow.h"
#include <QApplication>
#include <QDebug>

WindowFactory& WindowFactory::instance()
{
    static WindowFactory instance;
    return instance;
}

WindowFactory::WindowFactory(QObject *parent)
    : QObject(parent)
    , m_currentMainWindow(nullptr)
{
}

QMainWindow* WindowFactory::createMainWindow(int role, QWidget *parent)
{
    // 如果已有窗口，先销毁
    destroyCurrentWindow();

    QMainWindow *window = nullptr;

    switch(role) {
    case 1:  // 管理员
        window = new AdminMainWindow(parent);
        window->setWindowTitle("管理员界面 - 学生成绩管理系统");
        qDebug() << "创建管理员窗口";
        break;

    case 0:  // 普通用户
    default:
        window = new UserMainWindow();
        window->setWindowTitle("用户界面 - 学生成绩管理系统");
        qDebug() << "创建普通用户窗口";
        break;
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

QDialog* WindowFactory::createLoginWindow(QWidget *parent)
{
    LoginDlg *loginDlg = new LoginDlg(parent);
    loginDlg->setWindowTitle("用户登录 - 学生成绩管理系统");

    // 连接登录成功信号
    connect(loginDlg, &LoginDlg::loginSuccess, this,
            [this, loginDlg](int userId, const QString &username, int role) {
                qDebug() << "登录成功，用户ID:" << userId << "用户名:" << username << "角色:" << role;

                // 创建对应的主窗口
                QMainWindow *mainWindow = createMainWindow(role);
                if (mainWindow) {
                    mainWindow->showMaximized();
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

void WindowFactory::switchToLogin()
{
    // 销毁当前主窗口
    destroyCurrentWindow();

    // 发射注销信号
    emit logoutRequested();

    // 创建并显示登录窗口
    QDialog *loginDlg = createLoginWindow();
    loginDlg->show();
}

QMainWindow* WindowFactory::currentMainWindow() const
{
    return m_currentMainWindow;
}

void WindowFactory::destroyCurrentWindow()
{
    if (m_currentMainWindow) {
        m_currentMainWindow->close();
        m_currentMainWindow->deleteLater();
        m_currentMainWindow = nullptr;
    }
}
