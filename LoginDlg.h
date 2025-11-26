#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include "DBOperator.h"

namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();

protected slots:
    void on_loginBtn_clicked();
    void on_regBtn_clicked();

private:
    Ui::LoginDlg *ui;
    DBOperator dbp;
};

#endif // LOGINDLG_H
