#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include "DBOperator.h"

#include <QDialog>

namespace Ui { class RegDlg; }

class RegDlg : public QDialog {
    Q_OBJECT

public:
    explicit RegDlg(QWidget *parent = nullptr);
    ~RegDlg();

private slots:
    void on_regBtn_clicked();
    void on_backBtn_clicked();

private:
    bool validateInput(QString &username, QString &password, QString &repassword, QString &phone, QString &email, QString &realname, QString &idcard);

    Ui::RegDlg *ui;
    DBOperator dbp;
};

#endif // REGISTERDLG_H
