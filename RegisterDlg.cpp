#include "RegisterDlg.h"
#include "ui_RegisterDlg.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

// 构造函数：初始化注册对话框
RegDlg::RegDlg(QWidget *parent) : QDialog(parent), ui(new Ui::RegDlg) {
    ui->setupUi(this);
    // 设置 Tab 键顺序
    QWidget::setTabOrder(ui->UsernameEdit, ui->PasswordEdit);
    QWidget::setTabOrder(ui->PasswordEdit, ui->RePasswordEdit);
    QWidget::setTabOrder(ui->RePasswordEdit, ui->PhoneEdit);
    QWidget::setTabOrder(ui->PhoneEdit, ui->EmailEdit);
    QWidget::setTabOrder(ui->EmailEdit, ui->RealnameEdit);
    QWidget::setTabOrder(ui->RealnameEdit, ui->IdcardEdit);
    QWidget::setTabOrder(ui->IdcardEdit, ui->regBtn);
    QWidget::setTabOrder(ui->regBtn, ui->backBtn);
}

RegDlg::~RegDlg() {
    delete ui;
}

// 验证用户输入信息的合法性
bool RegDlg::validateInput(QString &username, QString &password, QString &repassword, QString &phone, QString &email, QString &realname, QString &idcard) {
    // 获取并去除输入框中的空格
    username = ui->UsernameEdit->text().trimmed();
    password = ui->PasswordEdit->text().trimmed();
    repassword = ui->RePasswordEdit->text();
    phone = ui->PhoneEdit->text().trimmed();
    email = ui->EmailEdit->text().trimmed();
    realname = ui->RealnameEdit->text().trimmed();
    idcard = ui->IdcardEdit->text().trimmed();

    // 验证各项输入是否合法
    if (username.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名不能为空");
        return false;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "警告", "密码不能为空");
        return false;
    }
    if (password != repassword) {
        QMessageBox::warning(this, "警告", "两次输入的密码不一致");
        return false;
    }
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "警告", "手机号不能为空");
        return false;
    }
    if (phone.length() != 11 || !phone.toLongLong()) {
        QMessageBox::warning(this, "警告", "手机号格式不正确，请输入11位数字");
        return false;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "警告", "邮箱不能为空");
        return false;
    }
    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "警告", "邮箱格式不正确");
        return false;
    }
    if (realname.isEmpty()) {
        QMessageBox::warning(this, "警告", "真实姓名不能为空");
        return false;
    }
    if (idcard.isEmpty()) {
        QMessageBox::warning(this, "警告", "身份证号不能为空");
        return false;
    }
    if (idcard.length() != 18) {
        QMessageBox::warning(this, "警告", "身份证号格式不正确，请输入18位身份证号");
        return false;
    }
    return true;
}

// 处理注册按钮点击
void RegDlg::on_regBtn_clicked() {
    QString username, password, repassword, phone, email, realname, idcard;

    // 验证输入合法性
    if (!validateInput(username, password, repassword, phone, email, realname, idcard)) {
        return;
    }

    // 检查用户名是否已被注册（用户表）
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

    // 检查用户名是否已被注册（管理员表）
    checkSql = QString("select count(1) from admin_info where username='%1'").arg(username);
    checkQuery = dbp.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "注册失败", "该用户名已存在，请重新输入");
        return;
    }

    // 检查手机号是否已被注册
    checkSql = QString("select count(1) from user_info where phone='%1'").arg(phone);
    checkQuery = dbp.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "注册失败", "该手机号已被注册，请重新输入");
        return;
    }

    // 检查邮箱是否已被注册
    checkSql = QString("select count(1) from user_info where email='%1'").arg(email);
    checkQuery = dbp.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "注册失败", "该邮箱已被注册，请重新输入");
        return;
    }

    // 检查身份证号是否已被注册
    checkSql = QString("select count(1) from user_info where idcard='%1'").arg(idcard);
    checkQuery = dbp.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "注册失败", "该身份证号已被注册，请重新输入");
        return;
    }

    // 插入新用户信息到数据库
    QString insertSql = QString("insert into user_info(username, password, phone, email, realname, idcard, avatarid) values('%1', '%2', '%3', '%4', '%5', '%6', 1)").arg(username, password, phone, email, realname, idcard);
    QSqlQuery insertQuery = dbp.DBGetData(insertSql, success);
    if (!success) {
        QMessageBox::warning(this, "注册失败", insertQuery.lastError().text());
        return;
    }

    QMessageBox::information(this, "注册成功", "注册成功，请返回登录");
    accept();
}

// 处理返回按钮点击，取消注册
void RegDlg::on_backBtn_clicked() {
    reject();
}
