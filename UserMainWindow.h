#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>

#include "DBOperator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserMainWindow; }
QT_END_NAMESPACE

class UserMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit UserMainWindow(int userId = 0, QWidget *parent = nullptr);
    ~UserMainWindow();

signals:
    void logoutRequested();

private slots:
    // 页面切换相关
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

    // 航班查询相关
    void on_searchBtn_clicked();
    void on_resetBtn_clicked();
    void on_book_clicked(const QString& flightNo);
    void on_collect_clicked(const QString& flightNo);

    // 个人中心相关
    void on_editInfoBtn_clicked();
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();
    void on_cancelAccountBtn_clicked();
    void on_rechargeBtn_clicked();

private:
    // UI 相关
    Ui::UserMainWindow *ui;

    // 基础数据
    int m_userId;
    bool resetStatus;
    DBOperator m_dbOperator;

    // 布局相关
    QVBoxLayout *flightLayout;
    QVBoxLayout *orderLayout;
    QVBoxLayout *favoritesLayout;

    // 编辑模式相关成员变量
    bool m_isEditMode;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_realnameEdit;
    QLineEdit *m_idcardEdit;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
    DBOperator::UserInfo m_originalUserInfo;

    // 事件处理
    bool eventFilter(QObject *obj, QEvent *event) override;

    // 用户信息相关
    void loadAvatar();
    void setAvatar(int avatarId);
    void showAvatarselectionDialog();
    void loadUserInfo();
    void enterEditMode();
    void exitEditMode();
    void saveUserInfo();

    // 航班相关
    void clearFlightItems();
    void loadAllFlights();

    // 订单相关
    void clearOrders();
    void loadOrders();
    void handleCancelOrder(int orderId, const QString& flightId, double price);
    void handleReschedule(int orderId, const QString& oldFlightId, double oldPrice, const QString& departureCity, const QString& arrivalCity);

    // 收藏相关
    void clearFavorites();
    void loadFavorites();
    void handleUnfavorite(const QString& flightId);

    // 工具函数
    QString formatDateTime(const QDateTime& dateTime);
    void fillComboBox(QComboBox *cbox, const QString &sql);
};
#endif // USERMAINWINDOW_H
