#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>

#include "DBOperator.h"

namespace Ui { class LoginDlg; }

class LoginDlg : public QDialog {
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();

    // 工具函数
    void clearInput();

signals:
    void loginSuccess(int userId, const QString &username, bool isAdmin);

private slots:
    void on_loginBtn_clicked();
    void on_regBtn_clicked();
    void on_exitBtn_clicked();

private:
    Ui::LoginDlg *ui;
    DBOperator dbp;
};

#endif // LOGINDLG_H
