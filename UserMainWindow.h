#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

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
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

    void on_searchBtn_clicked();
    void on_resetBtn_clicked();
    void on_book_clicked(const QString& flightNo);
    void on_collect_clicked(const QString& flightNo);
    void on_editInfoBtn_clicked();
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();
    void on_cancelAccountBtn_clicked();
    void on_rechargeBtn_clicked();

private:
    Ui::UserMainWindow *ui;
    int m_userId;
    DBOperator m_dbOperator;
    QVBoxLayout *flightLayout;
    QVBoxLayout *orderLayout; // 订单列表布局
    QVBoxLayout *favoritesLayout; // 收藏列表布局

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

    // 保存原始数据，用于取消时恢复
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
};
#endif // USERMAINWINDOW_H
