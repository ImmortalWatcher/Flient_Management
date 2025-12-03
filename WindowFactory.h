#ifndef WINDOWFACTORY_H
#define WINDOWFACTORY_H

#include <QMainWindow>
#include <QObject>

// 前向声明，避免循环包含
class UserMainWindow;
class AdminMainWindow;

class WindowFactory : public QObject {
    Q_OBJECT
public:
    // 禁止拷贝和赋值
    WindowFactory(const WindowFactory&) = delete;
    WindowFactory& operator=(const WindowFactory&) = delete;

    // 获取单例实例
    static WindowFactory& instance();

    /**
     * @brief 创建主窗口
     * @param isAdmin 是否为管理员 (true: 管理员, false: 普通用户)
     * @param parent 父窗口
     * @return 创建的主窗口指针
     */
    QMainWindow* createMainWindow(bool isAdmin, QWidget *parent = nullptr);

    /**
     * @brief 创建登录窗口
     * @param parent 父窗口
     * @return 登录窗口指针
     */
    QDialog* createLoginWindow(QWidget *parent = nullptr);

    /**
     * @brief 切换到登录窗口（用于注销）
     */
    void switchToLogin();

    /**
     * @brief 获取当前主窗口
     */
    QMainWindow* currentMainWindow() const;

    /**
     * @brief 销毁当前主窗口
     */
    void destroyCurrentWindow();

signals:
    // 窗口创建信号
    void mainWindowCreated(QMainWindow *window);
    // 注销请求信号
    void logoutRequested();

private:
    explicit WindowFactory(QObject *parent = nullptr);
    ~WindowFactory() = default;

    // 当前活动的主窗口
    QMainWindow *m_currentMainWindow = nullptr;
};

#endif // WINDOWFACTORY_H
