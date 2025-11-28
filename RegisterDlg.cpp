#include "RegisterDlg.h"
#include "ui_RegisterDlg.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

RegDlg::RegDlg(QWidget *parent) : QDialog(parent), ui(new Ui::RegDlg) {
    ui->setupUi(this);
}

RegDlg::~RegDlg() { delete ui; }

// 输入合法性校验
bool RegDlg::validateInput(QString &username, QString &password, QString &repassword) {
    username = ui->AccountEdit->text().trimmed();
    password = ui->PasswordEdit->text();
    repassword = ui->RePasswordEdit->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "注册", "用户名不能为空");
        return false;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "注册", "密码不能为空");
        return false;
    }
    if (password != repassword) {
        QMessageBox::warning(this, "注册", "两次输入的密码不一致");
        return false;
    }
    return true;
}

void RegDlg::on_regBtn_clicked() {
    QString username; QString password; QString repassword;

    if (!validateInput(username, password, repassword)) {
        return;
    }

    // 检查用户名是否已被注册
    bool success = false;
    QString checkSql = QString("select count(1) from user_info where username='%1'").arg(username);
    QSqlQuery checkQuery = dbp.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "注册失败", "该用户名已存在，请重新输入");
        return;
    }

    // 构造 SQL 语句并执行插入
    QString insertSql = QString("insert into user_info(username, password) values('%1', '%2')").arg(username, password);
    QSqlQuery insertQuery = dbp.DBGetData(insertSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", insertQuery.lastError().text());
        return;
    }

    // 注册成功, 弹出提示窗口, 并向父类返回状态
    QMessageBox::information(this, "注册成功", "注册成功，请返回登录");
    accept();
}

void RegDlg::on_backBtn_clicked() { reject(); }
