#include "LoginDlg.h"
#include "ui_LoginDlg.h"
#include "regdlg.h"
#include <QTimer>
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QMessageBox>
LoginDlg::LoginDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    dbp.DBOpen();
}

LoginDlg::~LoginDlg()
{
    dbp.DBClose();
    delete ui;
}

void LoginDlg::on_loginBtn_clicked()
{
    // qDebug()<<__FUNCTION__;
    auto username=ui->AccountEdit->text();
    auto password=ui->PasswordEdit->text();
    if (username.isEmpty()||password.isEmpty())
    {
        qWarning()<<"账号或密码不能为空";
        return;
    }
    bool sf=false;
    QString sqlstr = QString("select * from user_info where username='%1' and password='%2'").arg(username).arg(password);
    qDebug() << "执行的SQL语句:" << sqlstr;
    QSqlQuery qs = dbp.DBGetData(sqlstr, sf);
    //query.bindValue(0,username);
    //query.bindValue(1,password);
    if (!sf)
    {
        QMessageBox::warning(this,"登录",qs.lastError().text());
        return;
    }
    if (qs.next())
    {
        done(QDialog::Accepted);
    }
    else
    {
        qWarning()<<"账号或密码错误";
    }
}
void LoginDlg::on_regBtn_clicked()
{
    this->hide();
    // RegDlg regWindow;
    // regWindow.setModal(true);
    // regWindow.show();
    // this->activateWindow();
    this->show();
}
