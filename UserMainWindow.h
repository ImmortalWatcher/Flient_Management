#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include "LoginDlg.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class UserMainWindow; }
QT_END_NAMESPACE

class UserMainWindow : public QMainWindow {
    Q_OBJECT

public:
    // UserMainWindow(LoginDlg *loginDlg = nullptr, QWidget *parent = nullptr);
    UserMainWindow(QWidget *parent = nullptr);
    ~UserMainWindow();

private slots:
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

private:
    Ui::UserMainWindow *ui;
    LoginDlg *m_loginDlg;
};
#endif // USERMAINWINDOW_H
