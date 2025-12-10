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

    void loadAvatar();
    void showAvatarselectionDialog();
    void setAvatar(int avatarId);
    void loadUserInfo();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void clearFlightItems();
    void loadAllFlights();
    void enterEditMode();
    void exitEditMode();
    void saveUserInfo();
};
#endif // USERMAINWINDOW_H
