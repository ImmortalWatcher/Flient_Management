#include "LoginDlg.h"
#include "ui_LoginDlg.h"

#include "RegisterDlg.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

// 构造函数：初始化登录对话框
LoginDlg::LoginDlg(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDlg) {
    ui->setupUi(this);
    // 设置 Tab 键顺序
    QWidget::setTabOrder(ui->UsernameEdit, ui->PasswordEdit);
    QWidget::setTabOrder(ui->PasswordEdit, ui->loginBtn);
    QWidget::setTabOrder(ui->loginBtn, ui->regBtn);
    dbp.DBOpen();
}

LoginDlg::~LoginDlg() {
    dbp.DBClose();
    delete ui;
}

// 清空输入框
void LoginDlg::clearInput() {
    ui->UsernameEdit->clear();
    ui->PasswordEdit->clear();
}

// 处理登录按钮点击
void LoginDlg::on_loginBtn_clicked() {
    QString username = ui->UsernameEdit->text();
    QString password = ui->PasswordEdit->text();

    // 验证输入是否为空
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名或密码不能为空！");
        return;
    }

    // 先查询用户表
    bool sf = false;
    QString sqlstr = QString("select * from user_info where username='%1' AND password='%2'").arg(username, password);
    QSqlQuery qs = dbp.DBGetData(sqlstr, sf);

    if (!sf) {
        QMessageBox::warning(this, "登录失败", qs.lastError().text());
        return;
    }

    bool isAdmin = false;
    int userId = 0;
    QString dbUsername;

    // 如果用户在用户表中找到，则为普通用户
    if (qs.next()) {
        userId = qs.value("id").toInt();
        dbUsername = qs.value("username").toString();
        isAdmin = false;
    } else {
        // 如果用户表中没找到，查询管理员表
        sqlstr = QString("select * from admin_info where username='%1' AND password='%2'").arg(username, password);
        qs = dbp.DBGetData(sqlstr, sf);

        if (!sf) {
            QMessageBox::warning(this, "登录失败", qs.lastError().text());
            return;
        }

        if (qs.next()) {
            userId = qs.value("id").toInt();
            dbUsername = qs.value("username").toString();
            isAdmin = true;
        } else {
            QMessageBox::warning(this, "登录失败", "用户名或密码错误，请重新输入！");
            return;
        }
    }

    this->hide();
    emit loginSuccess(userId, dbUsername, isAdmin);
    QDialog::accept();
}

// 处理注册按钮点击，打开注册对话框
void LoginDlg::on_regBtn_clicked() {
    RegDlg regDialog(this);
    regDialog.setGeometry(this->geometry());
    regDialog.exec();
}
