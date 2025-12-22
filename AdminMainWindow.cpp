#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"

#include <QAbstractItemView>
#include <QDate>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDialog>
#include <QBrush>
#include <QColor>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QMap>
#include <QToolTip>
#include <QCursor>

// 构造函数：初始化管理员主窗口
AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
    dbOperator = new DBOperator();
    dbOperator->DBOpen();

    totalStatsChart = nullptr;
    airlineFlightChart = nullptr;
    airlineOrderChart = nullptr;
    cityFlightChart = nullptr;
    cityOrderChart = nullptr;
    totalStatsView = nullptr;
    airlineFlightView = nullptr;
    airlineOrderView = nullptr;
    cityFlightView = nullptr;
    cityOrderView = nullptr;

    // 初始化各个功能模块
    initFlightManagement();
    initOrderView();
    initUserManagement();
    initDataStatistics();
}

AdminMainWindow::~AdminMainWindow() {
    delete totalStatsView;
    delete airlineFlightView;
    delete airlineOrderView;
    delete cityFlightView;
    delete cityOrderView;

    delete totalStatsChart;
    delete airlineFlightChart;
    delete airlineOrderChart;
    delete cityFlightChart;
    delete cityOrderChart;

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

    //  设置航班表格选中高亮配置
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->flightTable->setStyleSheet(
        "QTableWidget::item:selected {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: 1px solid #42a5f5;"
        "}"
        "QTableWidget::item:selected:active {"
        "    background-color: #64b5f6;"
        "    color: #000000;"
        "    border: 1px solid #2196f3;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #90caf9;"
        "    selection-color: #000000;"
        "}"
        "QTableWidget::item {"
        "    padding: 2px;"
        "}"
        // 表头居中 (可选，和用户表格保持一致)
        "QHeaderView::section {"
        "    text-align: center;"
        "}"
        );
    // 表头文本居中对齐 (可选)
    QHeaderView *flightHeader = ui->flightTable->horizontalHeader();
    if (flightHeader) {
        flightHeader->setDefaultAlignment(Qt::AlignCenter);
    }
    // 列宽自适应内容 (可选)
    ui->flightTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    loadFlightData();
}

// 初始化订单查看模块
void AdminMainWindow::initOrderView() {
    orderModel = new QSqlQueryModel(this);

    ui->twOrderList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twOrderList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrderList->setStyleSheet(
        "QTableWidget::item:selected {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: 1px solid #42a5f5;"
        "}"
        "QTableWidget::item:selected:active {"
        "    background-color: #64b5f6;"
        "    color: #000000;"
        "    border: 1px solid #2196f3;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #90caf9;"
        "    selection-color: #000000;"
        "}"
        "QTableWidget::item {"
        "    padding: 2px;"
        "}"
        "QHeaderView::section {"
        "    text-align: center;"
        "}"
        );
    ui->twOrderList->setColumnCount(12);
    QStringList orderHeaders = {
        "订单号", "用户ID", "航班号", "乘客姓名", "身份证号",
        "出发城市", "起飞时间", "到达城市", "到达时间", "票价",
        "下单时间", "订单状态"
    };
    ui->twOrderList->setHorizontalHeaderLabels(orderHeaders);
    QHeaderView *orderHeader = ui->twOrderList->horizontalHeader();
    if (orderHeader) {
        orderHeader->setDefaultAlignment(Qt::AlignCenter);
        orderHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    ui->cbOrderStatus->clear();
    ui->cbOrderStatus->addItem("全部订单");
    ui->cbOrderStatus->addItem("已支付");
    ui->cbOrderStatus->addItem("已取消");

    loadOrderData();
}

// 初始化用户管理模块
void AdminMainWindow::initUserManagement() {
    userModel = new QSqlQueryModel(this);
    ui->twUserList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twUserList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twUserList->setStyleSheet(
        "QTableWidget::item:selected {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: 1px solid #42a5f5;"
        "}"
        "QTableWidget::item:selected:active {"
        "    background-color: #64b5f6;"
        "    color: #000000;"
        "    border: 1px solid #2196f3;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #90caf9;"
        "    selection-color: #000000;"
        "}"
        "QTableWidget::item {"
        "    padding: 2px;"
        "}"
    );

    loadUserData();
}

// 初始化数据统计模块
void AdminMainWindow::initDataStatistics() {
    QStackedWidget *stackWidget = ui->stackedWidget;
    if (!stackWidget) {
        qWarning() << "错误：未找到 StackedWidget 控件！";
        return;
    }
    QWidget *statsPage = stackWidget->widget(3);
    if (!statsPage) {
        qWarning() << "错误：StackedWidget 索引 3 对应的页面不存在！";
        return;
    }

    QScrollArea *scrollArea = new QScrollArea(statsPage);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollArea->setWidget(scrollContent);

    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setSpacing(40);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *totalTitle = new QLabel("核心运营数据总览", scrollContent);
    totalTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(totalTitle, 0, Qt::AlignCenter);

    totalStatsChart = new QChart();
    totalStatsChart->setTitle("总航班数 / 总用户数 / 总订单数 / 订单总金额");
    totalStatsChart->setAnimationOptions(QChart::SeriesAnimations);
    totalStatsView = new QChartView(totalStatsChart);
    totalStatsView->setRenderHint(QPainter::Antialiasing);
    totalStatsView->setMinimumHeight(400);
    totalStatsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    totalStatsView->setStyleSheet("border: 1px solid #ecf0f1; border-radius: 10px; padding: 10px;");
    mainLayout->addWidget(totalStatsView);

    QLabel *airlineTitle = new QLabel("各航空公司航班占比", scrollContent);
    airlineTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(airlineTitle, 0, Qt::AlignCenter);

    airlineFlightChart = new QChart();
    airlineFlightChart->setTitle("各航空公司航班数量占比（百分比）");
    airlineFlightChart->setAnimationOptions(QChart::SeriesAnimations);
    airlineFlightView = new QChartView(airlineFlightChart);
    airlineFlightView->setRenderHint(QPainter::Antialiasing);
    airlineFlightView->setMinimumHeight(400);
    airlineFlightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    airlineFlightView->setStyleSheet("border: 1px solid #ecf0f1; border-radius: 10px; padding: 10px;");
    mainLayout->addWidget(airlineFlightView);

    // 5.3 城市航班数饼状图
    QLabel *cityTitle = new QLabel("各城市出发航班占比", scrollContent);
    cityTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(cityTitle, 0, Qt::AlignCenter);

    cityFlightChart = new QChart();
    cityFlightChart->setTitle("各城市出发航班数量占比（百分比）");
    cityFlightChart->setAnimationOptions(QChart::SeriesAnimations);
    cityFlightView = new QChartView(cityFlightChart);
    cityFlightView->setRenderHint(QPainter::Antialiasing);
    cityFlightView->setMinimumHeight(400);
    cityFlightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cityFlightView->setStyleSheet("border: 1px solid #ecf0f1; border-radius: 10px; padding: 10px;");
    mainLayout->addWidget(cityFlightView);

    QVBoxLayout *pageLayout = new QVBoxLayout(statsPage);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);

    updateStatistics();
}

// 加载航班数据到表格
void AdminMainWindow::loadFlightData(const QString &whereClause) {
    QString sql = "select flight_id, airline_company, departure_city, departure_airport, departure_time, arrival_city, arrival_airport, arrival_time, price, total_seats, remaining_seats from flight_info";
    if (!whereClause.isEmpty()) {
        sql += " where " + whereClause;
    }
    sql += " order by departure_time";

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->flightTable->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->flightTable->insertRow(row);

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

            QPushButton *editBtn = new QPushButton("编辑");
            QPushButton *deleteBtn = new QPushButton("删除");
            QString flightId = values.at(0);

            connect(editBtn, &QPushButton::clicked, this, [this, flightId]() {
                onEditFlight(flightId);
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

// 加载订单数据
void AdminMainWindow::loadOrderData(const QString &status) {
    QString sql = "select order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, departure_time, arrival_city, arrival_time, price, order_time, order_status from order_info";

    QString filterStatus = status.trimmed();
    if (!filterStatus.isEmpty() && filterStatus != "全部订单") {
        sql += " where order_status = '" + filterStatus.replace("'", "''") + "'";
    }

    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->twOrderList->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->twOrderList->insertRow(row);

            for (int col = 0; col < 12; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                item->setTextAlignment(Qt::AlignCenter);
                
                if (col == 11) {
                    QString status = query.value(col).toString();
                    if (status == "已支付") {
                        item->setForeground(QBrush(QColor(0, 150, 0)));
                    } else if (status == "已取消") {
                        item->setForeground(QBrush(QColor(200, 0, 0)));
                    }
                }
                
                ui->twOrderList->setItem(row, col, item);
            }

            row++;
        }

        ui->twOrderList->resizeColumnsToContents();
    } else {
        QMessageBox::warning(this, "错误",
                             QString("加载订单数据失败：%1").arg(query.lastError().text()));
    }
}

// 加载用户数据到表格
void AdminMainWindow::loadUserData() {
    QString sql = "select username, password, phone, email,realname,idcard, balance from user_info";
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->twUserList->setRowCount(0);

        int row = 0;
        while (query.next()) {
            ui->twUserList->insertRow(row);

            for (int col = 0; col < 7; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                item->setTextAlignment(Qt::AlignCenter);
                ui->twUserList->setItem(row, col, item);
            }

            row++;
        }

        QHeaderView *header = ui->twUserList->horizontalHeader();
        if (header) {
            header->setDefaultAlignment(Qt::AlignCenter);
        }

        ui->twUserList->resizeColumnsToContents();
        ui->twUserList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        QMessageBox::warning(this, "错误", "加载用户数据失败");
    }
}

// 更新统计数据
void AdminMainWindow::updateStatistics() {
    qint64 totalFlights = 0;
    qint64 totalUsers = 0;
    qint64 totalOrders = 0;
    double totalAmount = 0.0;

    bool success;

    QSqlQuery flightQuery = dbOperator->DBGetData("select count(*) as flight_count from flight_info", success);
    if (success && flightQuery.next()) {
        totalFlights = flightQuery.value("flight_count").toLongLong();
    }

    QSqlQuery userQuery = dbOperator->DBGetData("select count(*) as user_count from user_info", success);
    if (success && userQuery.next()) {
        totalUsers = userQuery.value("user_count").toLongLong();
    }

    QSqlQuery orderQuery = dbOperator->DBGetData("select count(*) as order_count, COALESCE(SUM(price), 0) as total_price from order_info", success);
    if (success && orderQuery.next()) {
        totalOrders = orderQuery.value("order_count").toLongLong();
        totalAmount = orderQuery.value("total_price").toDouble();
    }

    totalStatsChart->removeAllSeries();
    QList<QAbstractAxis*> oldAxes = totalStatsChart->axes();
    for (QAbstractAxis* axis : oldAxes) {
        totalStatsChart->removeAxis(axis);
    }

    QBarSet *countSet = new QBarSet("数量");
    QBarSet *amountSet = new QBarSet("金额");
    *countSet << totalFlights << totalUsers << totalOrders << 0;
    *amountSet << 0 << 0 << 0 << totalAmount;
    countSet->setColor(QColor(52, 152, 219));
    amountSet->setColor(QColor(231, 76, 60));

    QBarSeries *countSeries = new QBarSeries();
    QBarSeries *amountSeries = new QBarSeries();
    countSeries->append(countSet);
    amountSeries->append(amountSet);
    countSeries->setLabelsVisible(false);
    amountSeries->setLabelsVisible(false);

    QStringList categories = {"总航班数", "总用户数", "总订单数", "订单总金额(元)"};
    QList<qreal> countValues = {static_cast<qreal>(totalFlights), static_cast<qreal>(totalUsers), static_cast<qreal>(totalOrders), 0.0};
    QList<qreal> amountValues = {0.0, 0.0, 0.0, totalAmount};

    connect(countSeries, &QBarSeries::hovered, this, [=](bool status, int index, QBarSet *barset) {
        if (status && index >= 0 && index < categories.size() && countValues[index] > 0) {
            QString tooltip = QString("%1\n数量：%2").arg(categories[index]).arg(QString::number(countValues[index], 'f', 0));
            QToolTip::showText(QCursor::pos(), tooltip);
        } else {
            QToolTip::hideText();
        }
    });

    connect(amountSeries, &QBarSeries::hovered, this, [=](bool status, int index, QBarSet *barset) {
        if (status && index >= 0 && index < categories.size() && amountValues[index] > 0) {
            QString tooltip = QString("%1\n金额：%2 元").arg(categories[index]).arg(QString::number(amountValues[index], 'f', 2));
            QToolTip::showText(QCursor::pos(), tooltip);
        } else {
            QToolTip::hideText();
        }
    });

    QBarCategoryAxis *totalXAxis = new QBarCategoryAxis();
    totalXAxis->append({"总航班数", "总用户数", "总订单数", "订单总金额(元)"});
    totalXAxis->setLabelsFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->addAxis(totalXAxis, Qt::AlignBottom);
    countSeries->attachAxis(totalXAxis);
    amountSeries->attachAxis(totalXAxis);

    QValueAxis *axisY_Count = new QValueAxis();
    axisY_Count->setTitleText("数量");
    axisY_Count->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Count->setLabelFormat("%d");
    axisY_Count->setTickType(QValueAxis::TicksFixed);
    axisY_Count->setTickInterval(1);
    qint64 maxCount = qMax(totalFlights, qMax(totalUsers, totalOrders));
    int countRangeMax = (maxCount <= 0) ? 10 : static_cast<int>(maxCount * 1.3);
    axisY_Count->setRange(0, countRangeMax);
    totalStatsChart->addAxis(axisY_Count, Qt::AlignLeft);
    countSeries->attachAxis(axisY_Count);

    QValueAxis *axisY_Amount = new QValueAxis();
    axisY_Amount->setTitleText("金额（元）");
    axisY_Amount->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Amount->setLabelFormat("%.0f");
    axisY_Amount->setTickType(QValueAxis::TicksFixed);
    axisY_Amount->setTickInterval(100);
    double amountRangeMax = (totalAmount <= 0) ? 1000 : totalAmount * 1.3;
    axisY_Amount->setRange(0, amountRangeMax);
    totalStatsChart->addAxis(axisY_Amount, Qt::AlignRight);
    amountSeries->attachAxis(axisY_Amount);

    totalStatsChart->addSeries(countSeries);
    totalStatsChart->addSeries(amountSeries);
    totalStatsChart->setTitle("核心运营数据总览（数量/金额双轴）");
    totalStatsChart->setAnimationOptions(QChart::SeriesAnimations);
    totalStatsChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->legend()->setAlignment(Qt::AlignBottom);
    totalStatsChart->setMargins(QMargins(20, 20, 20, 20));

    QList<QColor> pieColors = {QColor(46, 204, 113), QColor(155, 89, 182), QColor(231, 76, 60), QColor(241, 196, 15), QColor(52, 152, 219), QColor(127, 140, 141)};

    airlineFlightChart->removeAllSeries();
    QPieSeries *airlinePieSeries = new QPieSeries();
    int totalAirlineFlights = 0;

    QSqlQuery airFlightQuery = dbOperator->DBGetData("SELECT airline_company, COUNT(*) FROM flight_info GROUP BY airline_company", success);
    if (success) {
        while (airFlightQuery.next()) {
            totalAirlineFlights += airFlightQuery.value(1).toInt();
        }
        airFlightQuery.first();
        airFlightQuery.previous();

        int colorIdx = 0;
        while (airFlightQuery.next()) {
            QString airline = airFlightQuery.value(0).toString().trimmed();
            int cnt = airFlightQuery.value(1).toInt();
            if (airline.isEmpty() || cnt == 0) continue;

            double percent = (double)cnt / totalAirlineFlights * 100;
            QString label = QString("%1 (%2%)").arg(airline).arg(percent, 0, 'f', 1);
            QPieSlice *slice = airlinePieSeries->append(label, cnt);
            slice->setBrush(pieColors[colorIdx % pieColors.size()]);
            slice->setLabelFont(QFont("Microsoft YaHei", 11));
            slice->setLabelVisible(true);
            slice->setExploded(false);
            connect(slice, &QPieSlice::hovered, this, [=](bool state) {
                if (state) {
                    QString tooltip = QString("%1\n航班数：%2\n占比：%3%").arg(airline).arg(cnt).arg(percent, 0, 'f', 1);
                    QToolTip::showText(QCursor::pos(), tooltip);
                } else {
                    QToolTip::hideText();
                }
            });
            colorIdx++;
        }
    } else {
    }

    airlineFlightChart->addSeries(airlinePieSeries);
    airlineFlightChart->setTitle("各航空公司航班数量占比（百分比）");
    airlineFlightChart->setTitleFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    airlineFlightChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    airlineFlightChart->legend()->setAlignment(Qt::AlignRight);
    airlineFlightChart->createDefaultAxes();

    cityFlightChart->removeAllSeries();
    QPieSeries *cityPieSeries = new QPieSeries();
    int totalCityFlights = 0;

    QSqlQuery cityFlightQuery = dbOperator->DBGetData("SELECT departure_city, COUNT(*) FROM flight_info GROUP BY departure_city", success);
    if (success) {
        while (cityFlightQuery.next()) {
            totalCityFlights += cityFlightQuery.value(1).toInt();
        }
        cityFlightQuery.first();
        cityFlightQuery.previous();

        int colorIdx = 0;
        while (cityFlightQuery.next()) {
            QString city = cityFlightQuery.value(0).toString().trimmed();
            int cnt = cityFlightQuery.value(1).toInt();
            if (city.isEmpty() || cnt == 0) continue;

            double percent = (double)cnt / totalCityFlights * 100;
            QString label = QString("%1 (%2%)").arg(city).arg(percent, 0, 'f', 1);
            QPieSlice *slice = cityPieSeries->append(label, cnt);
            slice->setBrush(pieColors[colorIdx % pieColors.size()]);
            slice->setLabelFont(QFont("Microsoft YaHei", 11));
            slice->setLabelVisible(true);
            slice->setExploded(false);
            connect(slice, &QPieSlice::hovered, this, [=](bool state) {
                if (state) {
                    QString tooltip = QString("%1\n航班数：%2\n占比：%3%").arg(city).arg(cnt).arg(percent, 0, 'f', 1);
                    QToolTip::showText(QCursor::pos(), tooltip);
                } else {
                    QToolTip::hideText();
                }
            });
            colorIdx++;
        }
    } else {
    }

    cityFlightChart->addSeries(cityPieSeries);
    cityFlightChart->setTitle("各城市出发航班数量占比（百分比）");
    cityFlightChart->setTitleFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    cityFlightChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    cityFlightChart->legend()->setAlignment(Qt::AlignRight);
    cityFlightChart->createDefaultAxes();
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

// 处理用户操作下拉框选择变化
void AdminMainWindow::on_comboBox_currentIndexChanged(int index) {
    if (index == 2) {
        emit logoutRequested();
        this->close();
    } else if (index == 1) {
        QMessageBox::information(this, "提示", "修改密码功能待实现");
    }
}

// 编辑选中的航班
void AdminMainWindow::on_editBtn_clicked() {
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的航班");
        return;
    }

    QString flightId = ui->flightTable->item(row, 0)->text();
    onEditFlight(flightId);
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
    QString status = ui->cbOrderStatus->currentText();
    loadOrderData(status);
}

// 处理删除用户按钮点击
void AdminMainWindow::on_deleteUserBtn_clicked() {
    int row = ui->twUserList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的用户");
        return;
    }

    QString username = ui->twUserList->item(row, 0)->text();
    int ret = QMessageBox::question(this, "确认删除", QString("确定要删除用户 %1 吗？\n此操作不可恢复！").arg(username), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    QString sql = QString("delete from user_info where username = '%1'").arg(username);
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        loadUserData();
        QMessageBox::information(this, "删除成功", QString("用户 %1 已成功删除").arg(username));
    } else {
        QMessageBox::warning(this, "删除失败", "删除用户失败：" + query.lastError().text());
    }
}

void AdminMainWindow::on_findBtn_clicked() {
    QString departureCity = ui->departureLineEdit->text().trimmed();
    QString arrivalCity = ui->destinationLineEdit->text().trimmed();
    int year = ui->yearSpin->value();
    int month = ui->monthSpin->value();
    int day = ui->daySpin->value();

    QStringList conditions;

    if (!departureCity.isEmpty()) {
        conditions.append(QString("departure_city = '%1'").arg(departureCity));
    }

    if (!arrivalCity.isEmpty()) {
        conditions.append(QString("arrival_city = '%1'").arg(arrivalCity));
    }

    QDate selectedDate(year, month, day);
    if (selectedDate.isValid()) {
        QString dateStr = selectedDate.toString("yyyy-MM-dd");
        conditions.append(QString("DATE(departure_time) = '%1'").arg(dateStr));
    }

    QString whereClause = conditions.join(" AND ");
    loadFlightData(whereClause);

    if (ui->flightTable->rowCount() == 0) {
        QMessageBox::information(this, "查询结果", "未找到符合筛选条件的航班信息！");
    }
}

// 创建航班编辑对话框
QDialog *AdminMainWindow::createFlightEditDialog(const QString &flightId) {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("编辑航班信息");
    dialog->setModal(true);
    dialog->setFixedSize(600, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QGridLayout *formLayout = new QGridLayout();
    mainLayout->addLayout(formLayout);

    QLabel *lblFlightId = new QLabel("航班号：");
    QLineEdit *leFlightId = new QLineEdit();
    leFlightId->setReadOnly(true);
    formLayout->addWidget(lblFlightId, 0, 0);
    formLayout->addWidget(leFlightId, 0, 1);

    QLabel *lblAirline = new QLabel("航空公司：");
    QLineEdit *leAirline = new QLineEdit();
    formLayout->addWidget(lblAirline, 1, 0);
    formLayout->addWidget(leAirline, 1, 1);

    QLabel *lblDepartCity = new QLabel("出发城市：");
    QLineEdit *leDepartCity = new QLineEdit();
    formLayout->addWidget(lblDepartCity, 2, 0);
    formLayout->addWidget(leDepartCity, 2, 1);

    QLabel *lblDepartAirport = new QLabel("出发机场：");
    QLineEdit *leDepartAirport = new QLineEdit();
    formLayout->addWidget(lblDepartAirport, 3, 0);
    formLayout->addWidget(leDepartAirport, 3, 1);

    QLabel *lblDepartTime = new QLabel("出发时间：");
    QDateTimeEdit *dtEditDepart = new QDateTimeEdit();
    dtEditDepart->setDisplayFormat("yyyy-MM-dd hh:mm");
    dtEditDepart->setCalendarPopup(true);
    formLayout->addWidget(lblDepartTime, 4, 0);
    formLayout->addWidget(dtEditDepart, 4, 1);

    QLabel *lblArriveCity = new QLabel("到达城市：");
    QLineEdit *leArriveCity = new QLineEdit();
    formLayout->addWidget(lblArriveCity, 5, 0);
    formLayout->addWidget(leArriveCity, 5, 1);

    QLabel *lblArriveAirport = new QLabel("到达机场：");
    QLineEdit *leArriveAirport = new QLineEdit();
    formLayout->addWidget(lblArriveAirport, 6, 0);
    formLayout->addWidget(leArriveAirport, 6, 1);

    QLabel *lblArriveTime = new QLabel("到达时间：");
    QDateTimeEdit *dtEditArrive = new QDateTimeEdit();
    dtEditArrive->setDisplayFormat("yyyy-MM-dd hh:mm");
    dtEditArrive->setCalendarPopup(true);
    formLayout->addWidget(lblArriveTime, 7, 0);
    formLayout->addWidget(dtEditArrive, 7, 1);

    QLabel *lblPrice = new QLabel("价格（元）：");
    QDoubleSpinBox *dsbPrice = new QDoubleSpinBox();
    dsbPrice->setRange(0.0, 99999.99);
    dsbPrice->setDecimals(2);
    formLayout->addWidget(lblPrice, 8, 0);
    formLayout->addWidget(dsbPrice, 8, 1);

    QLabel *lblTotalSeats = new QLabel("总座位数：");
    QSpinBox *sbTotalSeats = new QSpinBox();
    sbTotalSeats->setRange(1, 999);
    formLayout->addWidget(lblTotalSeats, 9, 0);
    formLayout->addWidget(sbTotalSeats, 9, 1);

    QLabel *lblRemainSeats = new QLabel("剩余座位数：");
    QSpinBox *sbRemainSeats = new QSpinBox();
    sbRemainSeats->setRange(0, 999);
    formLayout->addWidget(lblRemainSeats, 10, 0);
    formLayout->addWidget(sbRemainSeats, 10, 1);
    QString sql = QString("select * from flight_info where flight_id = '%1'").arg(flightId);
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);
    if (success && query.next()) {
        leFlightId->setText(query.value("flight_id").toString());
        leAirline->setText(query.value("airline_company").toString());
        leDepartCity->setText(query.value("departure_city").toString());
        leDepartAirport->setText(query.value("departure_airport").toString());
        dtEditDepart->setDateTime(query.value("departure_time").toDateTime());
        leArriveCity->setText(query.value("arrival_city").toString());
        leArriveAirport->setText(query.value("arrival_airport").toString());
        dtEditArrive->setDateTime(query.value("arrival_time").toDateTime());
        dsbPrice->setValue(query.value("price").toDouble());
        sbTotalSeats->setValue(query.value("total_seats").toInt());
        sbRemainSeats->setValue(query.value("remaining_seats").toInt());
    } else {
        QMessageBox::warning(dialog, "错误", "加载航班数据失败！");
        dialog->close();
        return nullptr;
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("保存");
    QPushButton *btnCancel = new QPushButton("取消");
    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(btnSave, &QPushButton::clicked, this, [=]() {
        QMap<QString, QVariant> editData;
        editData["airline_company"] = leAirline->text().trimmed();
        editData["departure_city"] = leDepartCity->text().trimmed();
        editData["departure_airport"] = leDepartAirport->text().trimmed();
        editData["departure_time"] = dtEditDepart->dateTime().toString("yyyy-MM-dd hh:mm:ss");
        editData["arrival_city"] = leArriveCity->text().trimmed();
        editData["arrival_airport"] = leArriveAirport->text().trimmed();
        editData["arrival_time"] = dtEditArrive->dateTime().toString("yyyy-MM-dd hh:mm:ss");
        editData["price"] = dsbPrice->value();
        editData["total_seats"] = sbTotalSeats->value();
        editData["remaining_seats"] = sbRemainSeats->value();

        if (editData["airline_company"].toString().isEmpty() ||
            editData["departure_city"].toString().isEmpty() ||
            editData["arrival_city"].toString().isEmpty()) {
            QMessageBox::warning(dialog, "提示", "航空公司、出发城市、到达城市不能为空！");
            return;
        }
        if (editData["remaining_seats"].toInt() > editData["total_seats"].toInt()) {
            QMessageBox::warning(dialog, "提示", "剩余座位数不能超过总座位数！");
            return;
        }

        if (saveFlightEditData(flightId, editData)) {
            QMessageBox::information(dialog, "成功", "航班信息修改成功！");
            dialog->accept();
            loadFlightData();
        } else {
            QMessageBox::warning(dialog, "错误", "修改航班信息失败！");
        }
    });

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    return dialog;
}

// 保存编辑后的航班数据到数据库
bool AdminMainWindow::saveFlightEditData(const QString &flightId, const QMap<QString, QVariant> &editData) {
    QString sql = QString(
                      "update flight_info set "
                      "airline_company = '%1', "
                      "departure_city = '%2', "
                      "departure_airport = '%3', "
                      "departure_time = '%4', "
                      "arrival_city = '%5', "
                      "arrival_airport = '%6', "
                      "arrival_time = '%7', "
                      "price = %8, "
                      "total_seats = %9, "
                      "remaining_seats = %10 "
                      "where flight_id = '%11'"
                      ).arg(
                          editData["airline_company"].toString().replace("'", "''"), // 转义单引号
                          editData["departure_city"].toString().replace("'", "''"),
                          editData["departure_airport"].toString().replace("'", "''"),
                          editData["departure_time"].toString(),
                          editData["arrival_city"].toString().replace("'", "''"),
                          editData["arrival_airport"].toString().replace("'", "''"),
                          editData["arrival_time"].toString(),
                          QString::number(editData["price"].toDouble()),
                          QString::number(editData["total_seats"].toInt()),
                          QString::number(editData["remaining_seats"].toInt()),
                          flightId
                          );

    bool success;
    dbOperator->DBGetData(sql, success);
    return success;
}

// 编辑航班
void AdminMainWindow::onEditFlight(const QString &flightId) {
    QDialog *editDialog = createFlightEditDialog(flightId);
    if (editDialog) {
        editDialog->exec();
        delete editDialog;
    }
}
