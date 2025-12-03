#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include "DBOperator.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class UserMainWindow; }
QT_END_NAMESPACE

class UserMainWindow : public QMainWindow {
    Q_OBJECT

public:
    // UserMainWindow(LoginDlg *loginDlg = nullptr, QWidget *parent = nullptr);
    UserMainWindow(int userId = 0, QWidget *parent = nullptr);
    ~UserMainWindow();

private slots:
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

private:
    Ui::UserMainWindow *ui;
    int m_userId;                     // 当前用户 ID
    DBOperator m_dbOperator;          // 数据库操作对象

    // 头像相关方法
    void loadAvatar();                // 加载头像
    void showAvatarSelectionDialog(); // 显示头像选择对话框
    void setAvatar(int avatarId);     // 设置头像

    // 事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // USERMAINWINDOW_H
