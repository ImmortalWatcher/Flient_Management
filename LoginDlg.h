#ifndef LOGINDLG_H
#define LOGINDLG_H

#include "DBOperator.h"

#include <QDialog>

namespace Ui { class LoginDlg; }

class LoginDlg : public QDialog {
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();

    void clearInput(); // 声明清空输入的函数

signals:  // 添加这个信号声明
    void loginSuccess(int userId, const QString &username, bool isAdmin);

protected slots:
    void on_loginBtn_clicked();
    void on_regBtn_clicked();

private:
    Ui::LoginDlg *ui;
    DBOperator dbp;
};

#endif // LOGINDLG_H
