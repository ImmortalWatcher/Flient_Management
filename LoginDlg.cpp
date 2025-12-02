#include "LoginDlg.h"
#include "ui_LoginDlg.h"
#include "RegisterDlg.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

// 同时打开和关闭数据库
LoginDlg::LoginDlg(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDlg) {
    ui->setupUi(this);
    // 设计 Tab 导航
    QWidget::setTabOrder(ui->AccountEdit, ui->PasswordEdit);
    QWidget::setTabOrder(ui->PasswordEdit, ui->loginBtn);
    QWidget::setTabOrder(ui->loginBtn, ui->regBtn);
    dbp.DBOpen();
}

LoginDlg::~LoginDlg() {
    dbp.DBClose();
    delete ui;
}

// 实现清空输入的函数 (内部可访问私有 ui)
void LoginDlg::clearInput() {
    ui->AccountEdit->clear();  // 清空账号框
    ui->PasswordEdit->clear(); // 清空密码框
}

void LoginDlg::on_loginBtn_clicked() {
    // qDebug() <<__FUNCTION__;
    auto username = ui->AccountEdit->text();
    auto password = ui->PasswordEdit->text();

    // 空值校验
    if (username.isEmpty()||password.isEmpty()) {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // 构造 SQL 语句并执行查询
    bool sf = false;
    QString sqlstr = QString("select * from user_info where username='%1' and password='%2'").arg(username, password);
    // qDebug() << "执行的 SQL 语句:" << sqlstr;
    QSqlQuery qs = dbp.DBGetData(sqlstr, sf);

    //query.bindValue(0, username);
    //query.bindValue(1, password);

    // 检查查询是否执行成功
    if (!sf) {
        QMessageBox::warning(this, "登录失败", qs.lastError().text());
        return;
    }

    // 验证账号密码是否匹配
    if (qs.next()) {
        // 从查询结果中获取用户ID、用户名和角色
        // 注意：这里的字段名需要与您的数据库表结构匹配
        // 假设您的表中有 user_id, username, role 这些字段
        int userId = qs.value("id").toInt();  // 获取用户ID
        QString dbUsername = qs.value("username").toString();  // 获取用户名
        int role = qs.value("role").toInt();  // 获取角色（0: 普通用户, 1: 管理员）

        this->hide(); // 隐藏登录框

        // 发射信号，传递用户ID、用户名和角色
        emit loginSuccess(userId, dbUsername, role);

        QDialog::accept(); // 通知 main.cpp 登录成功

        // done(QDialog::Accepted);
    } else {
        QMessageBox::warning(this, "登录失败", "账号或密码错误，请重新输入！");
    }
}

void LoginDlg::on_regBtn_clicked() {
    RegDlg regDialog(this);
    regDialog.exec();
}
