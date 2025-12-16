#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"

#include <QDate>
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTableWidgetItem>

// 构造函数：初始化管理员主窗口
AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
    dbOperator = new DBOperator();
    dbOperator->DBOpen();

    // 初始化各个功能模块
    initFlightManagement();
    initOrderView();
    initUserManagement();
    initDataStatistics();
}

AdminMainWindow::~AdminMainWindow() {
    delete ui;
    delete dbOperator;
    delete flightModel;
    delete orderModel;
    delete userModel;
}

// 初始化航班管理模块
void AdminMainWindow::initFlightManagement() {
    flightModel = new QSqlQueryModel(this);

    // 设置日期选择器的范围
    ui->yearSpin->setRange(2000, 2100);
    ui->monthSpin->setRange(1, 12);
    ui->daySpin->setRange(1, 31);

    // 设置默认日期为当前日期
    QDate currentDate = QDate::currentDate();
    ui->yearSpin->setValue(currentDate.year());
    ui->monthSpin->setValue(currentDate.month());
    ui->daySpin->setValue(currentDate.day());

    loadFlightData();
}

// 初始化订单查看模块
void AdminMainWindow::initOrderView() {
    orderModel = new QSqlQueryModel(this);
    // 设置订单表格的列数和表头
    ui->twUserList->setColumnCount(7);
    ui->twUserList->setHorizontalHeaderLabels(
        QStringList() << "订单号" << "用户名" << "航班号" << "出发地" << "目的地" << "起飞时间" << "票价");
}

// 初始化用户管理模块
void AdminMainWindow::initUserManagement() {
    userModel = new QSqlQueryModel(this);
    // 设置用户表格的列数和表头
    ui->twUserList->setColumnCount(5);
    ui->twUserList->setHorizontalHeaderLabels(QStringList() << "用户名" << "性别" << "手机号" << "邮箱" << "账户余额");
    loadUserData();
}

// 初始化数据统计模块
void AdminMainWindow::initDataStatistics() {
    // 初始化日期选择下拉框 (年份、月份、日期)
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 10; i <= currentYear; i++) {
        ui->cbStartYear->addItem(QString::number(i));
        ui->cbEndYear->addItem(QString::number(i));
    }

    for (int i = 1; i <= 12; i++) {
        ui->cbStartMonth->addItem(QString::number(i));
        ui->cbEndMonth->addItem(QString::number(i));
    }

    for (int i = 1; i <= 31; i++) {
        ui->cbStartDay->addItem(QString::number(i));
        ui->cbEndDay->addItem(QString::number(i));
    }

    // 设置默认日期为当前日期
    ui->cbStartYear->setCurrentText(QString::number(currentYear));
    ui->cbEndYear->setCurrentText(QString::number(currentYear));
    ui->cbStartMonth->setCurrentText(QString::number(QDate::currentDate().month()));
    ui->cbEndMonth->setCurrentText(QString::number(QDate::currentDate().month()));
    ui->cbStartDay->setCurrentText(QString::number(QDate::currentDate().day()));
    ui->cbEndDay->setCurrentText(QString::number(QDate::currentDate().day()));

    updateStatistics();
}

// 加载航班数据到表格
void AdminMainWindow::loadFlightData(const QString &whereClause) {
    QString sql =
        "select flight_id, airline_company, departure_city, departure_airport, "
        "departure_time, arrival_city, arrival_airport, arrival_time, price, "
        "total_seats, remaining_seats from flight_info";
    // where 必须在 order by 之前
    if (!whereClause.isEmpty()) {
        sql += " where " + whereClause;
    }
    sql += " order by departure_time";

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        // 这里直接使用 query 填充表格，避免 setQuery 之后继续访问被移动的对象
        ui->flightTable->setRowCount(0);

        // 遍历查询结果，填充表格数据
        int row = 0;
        while (query.next()) {
            ui->flightTable->insertRow(row);

            // 按列名读取，避免顺序偏差
            QStringList values;
            values << query.value("flight_id").toString()
                   << query.value("airline_company").toString()
                   << query.value("departure_city").toString()
                   << query.value("departure_airport").toString()
                   << query.value("departure_time").toDateTime().toString("yyyy-MM-dd hh:mm")
                   << query.value("arrival_city").toString()
                   << query.value("arrival_airport").toString()
                   << query.value("arrival_time").toDateTime().toString("yyyy-MM-dd hh:mm")
                   << query.value("price").toString()
                   << query.value("total_seats").toString()
                   << query.value("remaining_seats").toString();

            for (int col = 0; col < values.size(); col++) {
                QTableWidgetItem *item = new QTableWidgetItem(values.at(col));
                ui->flightTable->setItem(row, col, item);
            }

            // 为每行添加编辑和删除按钮，对应表头的 “编辑” “删除” 列
            QPushButton *editBtn = new QPushButton("编辑");
            QPushButton *deleteBtn = new QPushButton("删除");

            QString flightId = values.at(0);

            connect(editBtn, &QPushButton::clicked, [this, flightId]() {
                Q_UNUSED(flightId);
            });
            connect(deleteBtn, &QPushButton::clicked, [this, flightId]() {
                if (QMessageBox::question(this, "确认", "确定要删除这条航班记录吗?") == QMessageBox::Yes) {
                    QString sql = QString("delete from flight_info where flight_id = '%1'").arg(flightId);
                    bool success;
                    dbOperator->DBGetData(sql, success);
                    if (success) {
                        loadFlightData();
                    } else {
                        QMessageBox::warning(this, "错误", "删除失败");
                    }
                }
            });

            ui->flightTable->setCellWidget(row, 11, editBtn);
            ui->flightTable->setCellWidget(row, 12, deleteBtn);

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载航班数据失败");
    }
}

// 加载订单数据到表格
void AdminMainWindow::loadOrderData(const QString &status) {
    // 使用 join 查询关联订单、用户和航班信息
    QString sql = "select o.id, u.username, f.flight_number, f.departure, f.destination, f.departure_time, f.price from orders o join users u on o.user_id = u.id join flights f on o.flight_id = f.id";

    // 如果指定了订单状态，则添加状态过滤条件
    if (!status.isEmpty() && status != "状态") {
        sql += " where o.status = '" + status + "'";
    }

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->twUserList->setRowCount(0);

        // 填充订单数据到表格
        int row = 0;
        while (query.next()) {
            ui->twUserList->insertRow(row);

            for (int col = 0; col < 7; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->twUserList->setItem(row, col, item);
            }

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载订单数据失败");
    }
}

// 加载用户数据到表格
void AdminMainWindow::loadUserData() {
    QString sql = "select username, gender, phone, email, balance from users";
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->twUserList->setRowCount(0);

        // 填充用户数据到表格
        int row = 0;
        while (query.next()) {
            ui->twUserList->insertRow(row);

            for (int col = 0; col < 5; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->twUserList->setItem(row, col, item);
            }

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载用户数据失败");
    }
}

// 更新统计数据
void AdminMainWindow::updateStatistics() {
    bool success;
    QSqlQuery query;

    // 统计总航班数
    QString flightSql = "select count(*) from flights";
    query = dbOperator->DBGetData(flightSql, success);
    if (success && query.next()) {
        ui->leTotalFlights->setText(query.value(0).toString());
    }

    // 统计总订单数
    QString orderSql = "select count(*) from orders";
    query = dbOperator->DBGetData(orderSql, success);
    if (success && query.next()) {
        ui->leTotalOrders->setText(query.value(0).toString());
    }

    // 统计总用户数
    QString userSql = "select count(*) from users";
    query = dbOperator->DBGetData(userSql, success);
    if (success && query.next()) {
        ui->leTotalUsers->setText(query.value(0).toString());
    }

    // 统计已支付订单总金额
    QString amountSql = "select sum(amount) from orders where status = '已支付'";
    query = dbOperator->DBGetData(amountSql, success);
    if (success && query.next()) {
        ui->leTotalAmount->setText(query.value(0).toString() + " 元");
    }
}

// 切换到航班管理页面
void AdminMainWindow::on_flightManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    loadFlightData();
}

// 切换到订单查看页面
void AdminMainWindow::on_orderViewBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadOrderData();
}

// 切换到用户管理页面
void AdminMainWindow::on_userManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadUserData();
}

// 切换到数据统计页面
void AdminMainWindow::on_dataStatisticsBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    updateStatistics();
}

// 退出登录
void AdminMainWindow::on_backBtn_clicked() {
    emit logoutRequested();
    this->close();
}

// 处理用户操作下拉框选择变化 (退出登录或修改密码)
void AdminMainWindow::on_comboBox_currentIndexChanged(int index) {
    if (index == 2) {
        emit logoutRequested();
        this->close();
    } else if (index == 1) {
        QMessageBox::information(this, "提示", "修改密码功能待实现");
        ui->cbUserOperation->setCurrentIndex(0);
    }
}

// 编辑选中的航班
void AdminMainWindow::on_editBtn_clicked() {
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的航班");
        return;
    }

    int flightId = ui->flightTable->item(row, 0)->text().toInt();
    QMessageBox::information(this, "提示", QString("编辑航班 ID: %1 的功能待实现").arg(flightId));
}

// 删除选中的航班
void AdminMainWindow::on_deleteBtn_clicked() {
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的航班");
        return;
    }

    QString flightId = ui->flightTable->item(row, 0)->text();
    if (QMessageBox::question(this, "确认", "确定要删除这条航班记录吗?") == QMessageBox::Yes) {
        QString sql = QString("delete from flight_info where flight_id = '%1'").arg(flightId);
        bool success;
        dbOperator->DBGetData(sql, success);
        if (success) {
            loadFlightData();
            QMessageBox::information(this, "成功", "航班删除成功");
        } else {
            QMessageBox::warning(this, "错误", "删除失败");
        }
    }
}

// 根据订单状态过滤订单列表
void AdminMainWindow::on_cbOrderStatus_currentIndexChanged(int index) {
    Q_UNUSED(index);
    QString status = ui->cbUserOperation->currentText();
    loadOrderData(status);
}

// 处理用户操作下拉框选择变化 (查看或删除用户)
void AdminMainWindow::on_cbUserOperation_currentIndexChanged(int index) {
    Q_UNUSED(index);
    QString operation = ui->cbUserOperation->currentText();
    if (operation == "查看") {
        int row = ui->twUserList->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "提示", "请先选择要查看的用户");
            ui->cbUserOperation->setCurrentIndex(0);
            return;
        }
        QString username = ui->twUserList->item(row, 0)->text();
        QMessageBox::information(this, "用户信息", QString("查看用户 %1 的详细信息功能待实现").arg(username));
        ui->cbUserOperation->setCurrentIndex(0);
    } else if (operation == "删除") {
        int row = ui->twUserList->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "提示", "请先选择要删除的用户");
            ui->cbUserOperation->setCurrentIndex(0);
            return;
        }
        QString username = ui->twUserList->item(row, 0)->text();
        if (QMessageBox::question(this, "确认", QString("确定要删除用户 %1 吗?").arg(username)) == QMessageBox::Yes) {
            QString sql = QString("delete from users where username = '%1'").arg(username);
            bool success;
            dbOperator->DBGetData(sql, success);
            if (success) {
                loadUserData();
                QMessageBox::information(this, "成功", "用户删除成功");
            } else {
                QMessageBox::warning(this, "错误", "删除失败");
            }
        }
        ui->cbUserOperation->setCurrentIndex(0);
    }
}
