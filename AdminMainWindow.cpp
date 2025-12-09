#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
    dbOperator = new DBOperator();
    dbOperator->DBOpen();

    // 初始化各模块
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

void AdminMainWindow::initFlightManagement() {
    flightModel = new QSqlQueryModel(this);

    // 设置日期选择框范围
    ui->yearSpin->setRange(2000, 2100);
    ui->monthSpin->setRange(1, 12);
    ui->daySpin->setRange(1, 31);

    // 设置当前日期
    QDate currentDate = QDate::currentDate();
    ui->yearSpin->setValue(currentDate.year());
    ui->monthSpin->setValue(currentDate.month());
    ui->daySpin->setValue(currentDate.day());

    // 加载航班数据
    loadFlightData();
}

void AdminMainWindow::initOrderView() {
    orderModel = new QSqlQueryModel(this);
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "订单号" << "用户名" << "航班号"
                                                             << "出发地" << "目的地" << "起飞时间" << "票价");
}

void AdminMainWindow::initUserManagement() {
    userModel = new QSqlQueryModel(this);
    ui->tableWidget_2->setColumnCount(5);
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "用户名" << "性别" << "手机号"
                                                               << "邮箱" << "账户余额");
    loadUserData();
}

void AdminMainWindow::initDataStatistics() {
    // 初始化日期选择框
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 10; i <= currentYear; ++i) {
        ui->comboBox_4->addItem(QString::number(i));
        ui->comboBox_7->addItem(QString::number(i));
    }

    for (int i = 1; i <= 12; ++i) {
        ui->comboBox_5->addItem(QString::number(i));
        ui->comboBox_8->addItem(QString::number(i));
    }

    for (int i = 1; i <= 31; ++i) {
        ui->comboBox_6->addItem(QString::number(i));
        ui->comboBox_9->addItem(QString::number(i));
    }

    // 默认选择当前日期
    ui->comboBox_4->setCurrentText(QString::number(currentYear));
    ui->comboBox_7->setCurrentText(QString::number(currentYear));
    ui->comboBox_5->setCurrentText(QString::number(QDate::currentDate().month()));
    ui->comboBox_8->setCurrentText(QString::number(QDate::currentDate().month()));
    ui->comboBox_6->setCurrentText(QString::number(QDate::currentDate().day()));
    ui->comboBox_9->setCurrentText(QString::number(QDate::currentDate().day()));

    // 更新统计数据
    updateStatistics();
}

void AdminMainWindow::loadFlightData(const QString &whereClause) {
    QString sql = "SELECT id, flight_number, departure, destination, departure_time, "
                  "arrival_time, total_seats, remaining_seats, price FROM flights";

    if (!whereClause.isEmpty()) {
        sql += " WHERE " + whereClause;
    }

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        flightModel->setQuery(std::move(query));  // 使用移动语义，消除警告
        ui->flightTable->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->flightTable->insertRow(row);

            // 插入数据
            for (int col = 0; col < 9; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->flightTable->setItem(row, col, item);
            }

            // 添加操作按钮
            QPushButton *editBtn = new QPushButton("编辑");
            QPushButton *deleteBtn = new QPushButton("删除");
            connect(editBtn, &QPushButton::clicked, [this, row]() {
                // 编辑操作
                int flightId = ui->flightTable->item(row, 0)->text().toInt();
                // 这里可以实现编辑逻辑
            });
            connect(deleteBtn, &QPushButton::clicked, [this, row]() {
                // 删除操作
                int flightId = ui->flightTable->item(row, 0)->text().toInt();
                if (QMessageBox::question(this, "确认", "确定要删除这条航班记录吗?") == QMessageBox::Yes) {
                    QString sql = QString("DELETE FROM flights WHERE id = %1").arg(flightId);
                    bool success;
                    dbOperator->DBGetData(sql, success);
                    if (success) {
                        loadFlightData();
                    } else {
                        QMessageBox::warning(this, "错误", "删除失败");
                    }
                }
            });

            ui->flightTable->setCellWidget(row, 9, editBtn);
            ui->flightTable->setCellWidget(row, 10, deleteBtn);

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载航班数据失败");
    }
}

void AdminMainWindow::loadOrderData(const QString &status) {
    QString sql = "SELECT o.id, u.username, f.flight_number, f.departure, f.destination, "
                  "f.departure_time, f.price "
                  "FROM orders o "
                  "JOIN users u ON o.user_id = u.id "
                  "JOIN flights f ON o.flight_id = f.id";

    if (!status.isEmpty() && status != "状态") {
        sql += " WHERE o.status = '" + status + "'";
    }

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->tableWidget->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->tableWidget->insertRow(row);

            for (int col = 0; col < 7; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->tableWidget->setItem(row, col, item);
            }

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载订单数据失败");
    }
}

void AdminMainWindow::loadUserData() {
    QString sql = "SELECT username, gender, phone, email, balance FROM users";
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->tableWidget_2->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->tableWidget_2->insertRow(row);

            for (int col = 0; col < 5; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->tableWidget_2->setItem(row, col, item);
            }

            row++;
        }
    } else {
        QMessageBox::warning(this, "错误", "加载用户数据失败");
    }
}

void AdminMainWindow::updateStatistics() {
    // 总航班数
    QString flightSql = "SELECT COUNT(*) FROM flights";
    bool success;
    QSqlQuery flightQuery = dbOperator->DBGetData(flightSql, success);
    if (success && flightQuery.next()) {
        ui->lineEdit_3->setText(flightQuery.value(0).toString());
    }

    // 总订单数
    QString orderSql = "SELECT COUNT(*) FROM orders";
    QSqlQuery orderQuery = dbOperator->DBGetData(orderSql, success);
    if (success && orderQuery.next()) {
        ui->lineEdit_4->setText(orderQuery.value(0).toString());
    }

    // 总用户数
    QString userSql = "SELECT COUNT(*) FROM users";
    QSqlQuery userQuery = dbOperator->DBGetData(userSql, success);
    if (success && userQuery.next()) {
        ui->lineEdit_5->setText(userQuery.value(0).toString());
    }

    // 订单总金额
    QString amountSql = "SELECT SUM(amount) FROM orders WHERE status = '已支付'";
    QSqlQuery amountQuery = dbOperator->DBGetData(amountSql, success);
    if (success && amountQuery.next()) {
        ui->lineEdit_6->setText(amountQuery.value(0).toString() + " 元");
    }
}

void AdminMainWindow::on_flightManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    loadFlightData();
}

void AdminMainWindow::on_orderViewBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadOrderData();
}

void AdminMainWindow::on_userManagementBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadUserData();
}

void AdminMainWindow::on_dataStatisticsBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    updateStatistics();
}

void AdminMainWindow::on_backBtn_clicked() {
    emit logoutRequested();
    this->close();
}

void AdminMainWindow::on_comboBox_currentIndexChanged(int index) {
    if (index == 2) { // 退出登录
        emit logoutRequested();
        this->close();
    } else if (index == 1) { // 修改密码
        // 这里可以实现修改密码的逻辑
        QMessageBox::information(this, "提示", "修改密码功能待实现");
        ui->comboBox->setCurrentIndex(0); // 回到初始状态
    }
}

void AdminMainWindow::on_editBtn_clicked() {
    // 获取选中的行
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的航班");
        return;
    }

    int flightId = ui->flightTable->item(row, 0)->text().toInt();
    // 这里可以实现编辑航班的逻辑
    QMessageBox::information(this, "提示", QString("编辑航班 ID: %1 的功能待实现").arg(flightId));
}

void AdminMainWindow::on_deleteBtn_clicked() {
    // 获取选中的行
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的航班");
        return;
    }

    int flightId = ui->flightTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "确认", "确定要删除这条航班记录吗?") == QMessageBox::Yes) {
        QString sql = QString("DELETE FROM flights WHERE id = %1").arg(flightId);
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

void AdminMainWindow::on_comboBox_2_currentIndexChanged(int index) {
    QString status = ui->comboBox_2->currentText();
    loadOrderData(status);
}

void AdminMainWindow::on_comboBox_3_currentIndexChanged(int index) {
    QString operation = ui->comboBox_3->currentText();
    if (operation == "查看") {
        int row = ui->tableWidget_2->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "提示", "请先选择要查看的用户");
            ui->comboBox_3->setCurrentIndex(0);
            return;
        }
        QString username = ui->tableWidget_2->item(row, 0)->text();
        QMessageBox::information(this, "用户信息", QString("查看用户 %1 的详细信息功能待实现").arg(username));
        ui->comboBox_3->setCurrentIndex(0);
    } else if (operation == "删除") {
        int row = ui->tableWidget_2->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "提示", "请先选择要删除的用户");
            ui->comboBox_3->setCurrentIndex(0);
            return;
        }
        QString username = ui->tableWidget_2->item(row, 0)->text();
        if (QMessageBox::question(this, "确认", QString("确定要删除用户 %1 吗?").arg(username)) == QMessageBox::Yes) {
            QString sql = QString("DELETE FROM users WHERE username = '%1'").arg(username);
            bool success;
            dbOperator->DBGetData(sql, success);
            if (success) {
                loadUserData();
                QMessageBox::information(this, "成功", "用户删除成功");
            } else {
                QMessageBox::warning(this, "错误", "删除失败");
            }
        }
        ui->comboBox_3->setCurrentIndex(0);
    }
}
