#include "LoginDlg.h"
#include "ui_LoginDlg.h"
#include "RegisterDlg.h"

#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

LoginDlg::LoginDlg(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDlg) {
    ui->setupUi(this);
    dbp.DBOpen();
}

LoginDlg::~LoginDlg() {
    dbp.DBClose();
    delete ui;
}

void LoginDlg::on_loginBtn_clicked() {
    // qDebug()<<__FUNCTION__;
    auto username = ui->AccountEdit->text();
    auto password = ui->PasswordEdit->text();

    // 空值校验
    if (username.isEmpty()||password.isEmpty()) {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // 构造 SQL 语句并执行查询
    bool sf = false;
    QString sqlstr = QString("select * from user_info where username='%1' and password='%2'").arg(username).arg(password);
    qDebug() << "执行的SQL语句:" << sqlstr;
    QSqlQuery qs = dbp.DBGetData(sqlstr, sf);

    //query.bindValue(0, username);
    //query.bindValue(1, password);

    // 检查查询是否成功
    if (!sf) {
        QMessageBox::warning(this, "登录失败", qs.lastError().text());
        return;
    }

    // 验证账号密码是否匹配
    if (qs.next()) {
        done(QDialog::Accepted);
    } else {
        QMessageBox::warning(this, "登录失败", "账号或密码错误，请重新输入！");
    }
}

void LoginDlg::on_regBtn_clicked() {
    this->hide();
    // RegDlg regWindow;
    // regWindow.setModal(true);
    // regWindow.show();
    // this->activateWindow();
    this->show();
}
