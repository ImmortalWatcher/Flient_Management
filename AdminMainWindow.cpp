#include "AdminMainWindow.h"
#include "ui_AdminMainWindow.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QCursor>
#include <QDate>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolTip>
#include <QVBoxLayout>

// 填充 ComboBox 选项
void AdminMainWindow::fillComboBox(QComboBox *cbox, const QString &sql) {
    cbox->clear();
    bool sf = false;
    QSqlQuery query = dbOperator->DBGetData(sql, sf);
    if (!sf) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }
    while (query.next()) {
        QString itemText = query.value(0).toString();
        cbox->addItem(itemText);
    }
}

// 构造函数
AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
    resetStatus=true;
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

    QString sql = "select distinct departure_city from flight_info";
    fillComboBox(ui->departureCbox, sql);
    sql = "select distinct arrival_city from flight_info";
    fillComboBox(ui->arrivalCbox, sql);
}

AdminMainWindow::~AdminMainWindow() {
    // delete totalStatsView;
    // delete airlineFlightView;
    // delete airlineOrderView;
    // delete cityFlightView;
    // delete cityOrderView;

    // delete totalStatsChart;
    // delete airlineFlightChart;
    // delete airlineOrderChart;
    // delete cityFlightChart;
    // delete cityOrderChart;

    delete ui;
    delete dbOperator;
    // delete flightModel;
    // delete orderModel;
    // delete userModel;
}

// 初始化航班管理模块
void AdminMainWindow::initFlightManagement() {
    flightModel = new QSqlQueryModel(this);

    // 设置日期选择器的范围
    ui->yearSpin->setRange(2000, 2100);
    ui->monthSpin->setRange(1, 12);
    ui->daySpin->setRange(1, 31);
    ui->yearSpin->setValue(2026);
    ui->monthSpin->setValue(1);
    ui->daySpin->setValue(1);

    //  设置航班表格选中高亮配置
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->flightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flightTable->setTextElideMode(Qt::ElideNone);
    ui->flightTable->setShowGrid(true);
    ui->flightTable->setAlternatingRowColors(false);
    ui->flightTable->setStyleSheet(
        "QTableWidget::item:selected {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget::item:selected:active {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget::item:focus {"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #90caf9;"
        "    selection-color: #000000;"
        "    outline: none;"
        "}"
        "QTableWidget::item {"
        "    padding: 2px;"
        "    border: none;"
        "}"
        "QHeaderView::section {"
        "    text-align: center;"
        "}"
        );

    // 表头文本居中对齐
    QHeaderView *flightHeader = ui->flightTable->horizontalHeader();
    if (flightHeader) {
        flightHeader->setDefaultAlignment(Qt::AlignCenter);
    }

    // 列宽自适应内容，确保内容完整显示
    ui->flightTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->flightTable->horizontalHeader()->setMinimumSectionSize(80);

    loadFlightData();
}

// 初始化订单查看模块
void AdminMainWindow::initOrderView() {
    orderModel = new QSqlQueryModel(this);

    ui->twOrderList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twOrderList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrderList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twOrderList->setTextElideMode(Qt::ElideNone);
    ui->twOrderList->setShowGrid(true);
    ui->twOrderList->setAlternatingRowColors(false);
    ui->twOrderList->setStyleSheet(
        "QTableWidget::item:selected {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget::item:selected:active {"
        "    background-color: #90caf9;"
        "    color: #000000;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget::item:focus {"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #90caf9;"
        "    selection-color: #000000;"
        "    outline: none;"
        "}"
        "QTableWidget::item {"
        "    padding: 2px;"
        "    border: none;"
        "}"
        "QHeaderView::section {"
        "    text-align: center;"
        "}"
        );
    ui->twOrderList->setColumnCount(12);
    QStringList orderHeaders = { "订单号", "用户ID", "航班号", "乘客姓名", "身份证号", "出发城市", "起飞时间", "到达城市", "到达时间", "票价", "下单时间", "订单状态" };
    ui->twOrderList->setHorizontalHeaderLabels(orderHeaders);
    QHeaderView *orderHeader = ui->twOrderList->horizontalHeader();
    if (orderHeader) {
        orderHeader->setDefaultAlignment(Qt::AlignCenter);
    }
    // 列宽自适应内容，确保内容完整显示
    ui->twOrderList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->twOrderList->horizontalHeader()->setMinimumSectionSize(80);

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

    // 城市航班数饼状图
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
        // 禁用 UI 更新以提高性能
        ui->flightTable->setUpdatesEnabled(false);
        ui->flightTable->setRowCount(0);

        // 先收集所有数据
        QVector<QStringList> allRows;
        while (query.next()) {
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
            allRows.append(values);
        }

        // 一次性设置行数
        int rowCount = allRows.size();
        ui->flightTable->setRowCount(rowCount);

        // 批量设置数据
        for (int row = 0; row < rowCount; row++) {
            const QStringList &values = allRows.at(row);
            for (int col = 0; col < values.size(); col++) {
                QTableWidgetItem *item = new QTableWidgetItem(values.at(col));
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                ui->flightTable->setItem(row, col, item);
            }
        }

        // 重新启用 UI 更新
        ui->flightTable->setUpdatesEnabled(true);
        ui->flightTable->viewport()->update();
    } else {
        QMessageBox::warning(this, "错误", "加载航班数据失败");
    }
}


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
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                
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
        QMessageBox::warning(this, "错误", QString("加载订单数据失败：%1").arg(query.lastError().text()));
    }
}

// 加载用户数据到表格
void AdminMainWindow::loadUserData() {
    QString sql = "select username, password, phone, email,realname,idcard, balance from user_info";
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        ui->twUserList->setRowCount(0);

        ui->twUserList->setStyleSheet("QTableWidget::item { white-space: normal; }");

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
            header->setSectionResizeMode(QHeaderView::ResizeToContents);
        }
        ui->twUserList->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        QMessageBox::warning(this, "错误", "加载用户数据失败");
    }
}

static qreal getAxisMax(qreal value, int base) {
    if (value <= 0) {
        return base * 1.2;
    }
    qreal scaledValue = value * 1.2;
    qreal axisMax = ceil(scaledValue / base) * base;
    return qMax(axisMax, static_cast<qreal>(base));
}

// 更新统计数据
void AdminMainWindow::updateStatistics() {
    qint64 totalFlights = 0;
    qint64 totalUsers = 0;
    qint64 totalOrders = 0;
    double totalAmount = 0.0;

    bool success;

    // 数据库查询 (保持原有逻辑)
    QSqlQuery flightQuery = dbOperator->DBGetData("select count(*) as flight_count from flight_info", success);
    if (success && flightQuery.next()) {
        totalFlights = flightQuery.value("flight_count").toLongLong();
    }

    QSqlQuery userQuery = dbOperator->DBGetData("select count(*) as user_count from user_info", success);
    if (success && userQuery.next()) {
        totalUsers = userQuery.value("user_count").toLongLong();
    }

    QSqlQuery orderQuery = dbOperator->DBGetData("select count(*) as order_count, coalesce(sum(price), 0) as total_price from order_info", success);
    if (success && orderQuery.next()) {
        totalOrders = orderQuery.value("order_count").toLongLong();
        totalAmount = orderQuery.value("total_price").toDouble();
    }

    for (QAbstractSeries *series : totalStatsChart->series()) {
        QBarSeries *barSeries = qobject_cast<QBarSeries*>(series);
        if (barSeries) {
            qDeleteAll(barSeries->barSets());
        }
        totalStatsChart->removeSeries(series);
        delete series;
    }

    QList<QAbstractAxis*> oldAxes = totalStatsChart->axes();
    for (QAbstractAxis* axis : oldAxes) {
        totalStatsChart->removeAxis(axis);
        delete axis;
    }

    QBarSet *countSet = new QBarSet("数量");
    *countSet << static_cast<qreal>(totalFlights) << static_cast<qreal>(totalUsers) << static_cast<qreal>(totalOrders) << 0.0;
    countSet->setColor(QColor(52, 152, 219));
    QBarSeries *countSeries = new QBarSeries();
    countSeries->append(countSet);
    countSeries->setLabelsVisible(false);

    QBarSet *amountSet = new QBarSet("金额");
    *amountSet << 0.0 << 0.0 << 0.0 << static_cast<qreal>(totalAmount);
    amountSet->setColor(QColor(231, 76, 60));
    QBarSeries *amountSeries = new QBarSeries();
    amountSeries->append(amountSet);
    amountSeries->setLabelsVisible(false);

    QStringList categories = {"总航班数", "总用户数", "总订单数", "订单总金额(元)"};
    QList<qreal> countValues = {static_cast<qreal>(totalFlights), static_cast<qreal>(totalUsers), static_cast<qreal>(totalOrders), 0.0};
    QList<qreal> amountValues = {0.0, 0.0, 0.0, totalAmount};

    connect(countSeries, &QBarSeries::hovered, this, [=](bool status, int index, QBarSet *barset) {
        if (status && index >= 0 && index < 3 && countValues[index] > 0) {
            QString tooltip = QString("%1\n数量：%2").arg(categories[index]).arg(QString::number(countValues[index], 'f', 0));
            QToolTip::showText(QCursor::pos(), tooltip);
        } else {
            QToolTip::hideText();
        }
    });

    connect(amountSeries, &QBarSeries::hovered, this, [=](bool status, int index, QBarSet *barset) {
        if (status && index == 3 && amountValues[index] > 0) {
            QString tooltip = QString("%1\n金额：%2 元").arg(categories[index]).arg(QString::number(amountValues[index], 'f', 2));
            QToolTip::showText(QCursor::pos(), tooltip);
        } else {
            QToolTip::hideText();
        }
    });

    totalStatsChart->addSeries(countSeries);
    totalStatsChart->addSeries(amountSeries);

    QBarCategoryAxis *totalXAxis = new QBarCategoryAxis();
    totalXAxis->append(categories);
    totalXAxis->setLabelsFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->addAxis(totalXAxis, Qt::AlignBottom);
    countSeries->attachAxis(totalXAxis);
    amountSeries->attachAxis(totalXAxis);

    // 数量轴
    QValueAxis *axisY_Count = new QValueAxis();
    axisY_Count->setTitleText("数量");
    axisY_Count->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Count->setLabelFormat("%d");
    axisY_Count->setTickType(QValueAxis::TicksFixed);
    axisY_Count->setTickInterval(4);
    qint64 maxCount = qMax<qint64>(0, qMax(totalFlights, qMax(totalUsers, totalOrders)));
    qreal countAxisMax = getAxisMax(static_cast<qreal>(maxCount), 4);
    axisY_Count->setRange(0, countAxisMax);
    totalStatsChart->addAxis(axisY_Count, Qt::AlignLeft);
    countSeries->attachAxis(axisY_Count);

    // 金额轴
    QValueAxis *axisY_Amount = new QValueAxis();
    axisY_Amount->setTitleText("金额（元）");
    axisY_Amount->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Amount->setLabelFormat("%.0f");
    axisY_Amount->setTickType(QValueAxis::TicksFixed);
    int amountTick = 10;
    if (totalAmount > 1000) amountTick = 100;
    if (totalAmount > 10000) amountTick = 1000;
    axisY_Amount->setTickInterval(amountTick);
    qreal safeTotalAmount = qMax(0.0, totalAmount);
    qreal amountAxisMax = getAxisMax(safeTotalAmount, 10);
    axisY_Amount->setRange(0, amountAxisMax);
    totalStatsChart->addAxis(axisY_Amount, Qt::AlignRight);
    amountSeries->attachAxis(axisY_Amount);

    if (totalStatsChart->metaObject()->indexOfMethod("setAxisY(QAbstractAxis*, QAbstractSeries*)") != -1) {
        totalStatsChart->setAxisY(axisY_Count, countSeries);
        totalStatsChart->setAxisY(axisY_Amount, amountSeries);
    }

    totalStatsChart->setTitle("核心运营数据总览（数量/金额双轴）");
    totalStatsChart->setAnimationOptions(QChart::SeriesAnimations);
    totalStatsChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->legend()->setAlignment(Qt::AlignBottom);
    totalStatsChart->setMargins(QMargins(20, 20, 20, 20));

    QList<QColor> pieColors = {QColor(46, 204, 113), QColor(155, 89, 182), QColor(231, 76, 60), QColor(241, 196, 15), QColor(52, 152, 219), QColor(127, 140, 141)};

    // 航空公司航班占比饼图
    airlineFlightChart->removeAllSeries();
    QPieSeries *airlinePieSeries = new QPieSeries();
    int totalAirlineFlights = 0;

    QSqlQuery airFlightQuery = dbOperator->DBGetData("select airline_company, count(*) from flight_info group by airline_company", success);
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
    }

    airlineFlightChart->addSeries(airlinePieSeries);
    airlineFlightChart->setTitle("各航空公司航班数量占比（百分比）");
    airlineFlightChart->setTitleFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    airlineFlightChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    airlineFlightChart->legend()->setAlignment(Qt::AlignRight);
    airlineFlightChart->createDefaultAxes();

    // 城市出发航班占比饼图
    cityFlightChart->removeAllSeries();
    QPieSeries *cityPieSeries = new QPieSeries();
    int totalCityFlights = 0;

    QSqlQuery cityFlightQuery = dbOperator->DBGetData("select departure_city, count(*) from flight_info group by departure_city", success);
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
    if (resetStatus) loadFlightData();
    else on_findBtn_clicked();
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
            if (resetStatus) loadFlightData();
            else on_findBtn_clicked();
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
    resetStatus=false;
    QString departureCity = ui->departureCbox->currentText();
    QString arrivalCity = ui->arrivalCbox->currentText();
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
        conditions.append(QString("date(departure_time) = '%1'").arg(dateStr));
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
            if (resetStatus) loadFlightData();
            else on_findBtn_clicked();
        } else {
            QMessageBox::warning(dialog, "错误", "修改航班信息失败！");
        }
    });

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    return dialog;
}

// 保存航班编辑数据
bool AdminMainWindow::saveFlightEditData(const QString &flightId, const QMap<QString, QVariant> &editData) {
    QString sql = QString("update flight_info set airline_company = '%1', departure_city = '%2', departure_airport = '%3', departure_time = '%4', arrival_city = '%5', arrival_airport = '%6', arrival_time = '%7', price = %8, total_seats = %9, remaining_seats = %10 where flight_id = '%11'").arg(
                          editData["airline_company"].toString().replace("'", "''"),
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

// 编辑选中的用户
void AdminMainWindow::on_deleteUserBtn_2_clicked() {
    int row = ui->twUserList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的用户");
        return;
    }

    QString username = ui->twUserList->item(row, 0)->text();
    QDialog *editDialog = createUserEditDialog(username);
    if (editDialog) {
        editDialog->exec();
        delete editDialog;
    }
}

// 创建用户编辑对话框
QDialog *AdminMainWindow::createUserEditDialog(const QString &username) {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("编辑用户信息");
    dialog->setModal(true);
    dialog->setFixedSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QGridLayout *formLayout = new QGridLayout();
    mainLayout->addLayout(formLayout);

    // 用户名字段 (只读)
    QLabel *lblUsername = new QLabel("用户名：");
    QLineEdit *leUsername = new QLineEdit();
    leUsername->setReadOnly(true);
    formLayout->addWidget(lblUsername, 0, 0);
    formLayout->addWidget(leUsername, 0, 1);

    // 密码
    QLabel *lblPassword = new QLabel("密码：");
    QLineEdit *lePassword = new QLineEdit();
    formLayout->addWidget(lblPassword, 1, 0);
    formLayout->addWidget(lePassword, 1, 1);

    // 电话
    QLabel *lblPhone = new QLabel("电话：");
    QLineEdit *lePhone = new QLineEdit();
    formLayout->addWidget(lblPhone, 2, 0);
    formLayout->addWidget(lePhone, 2, 1);

    // 邮箱
    QLabel *lblEmail = new QLabel("邮箱：");
    QLineEdit *leEmail = new QLineEdit();
    formLayout->addWidget(lblEmail, 3, 0);
    formLayout->addWidget(leEmail, 3, 1);

    // 真实姓名
    QLabel *lblRealname = new QLabel("真实姓名：");
    QLineEdit *leRealname = new QLineEdit();
    formLayout->addWidget(lblRealname, 4, 0);
    formLayout->addWidget(leRealname, 4, 1);

    // 身份证号
    QLabel *lblIdcard = new QLabel("身份证号：");
    QLineEdit *leIdcard = new QLineEdit();
    formLayout->addWidget(lblIdcard, 5, 0);
    formLayout->addWidget(leIdcard, 5, 1);

    // 余额
    QLabel *lblBalance = new QLabel("余额（元）：");
    QDoubleSpinBox *dsbBalance = new QDoubleSpinBox();
    dsbBalance->setRange(0.0, 99999.99);
    dsbBalance->setDecimals(2);
    formLayout->addWidget(lblBalance, 6, 0);
    formLayout->addWidget(dsbBalance, 6, 1);

    // 加载用户数据
    QString sql = QString("select * from user_info where username = '%1'").arg(username);
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);
    if (success && query.next()) {
        leUsername->setText(query.value("username").toString());
        lePassword->setText(query.value("password").toString());
        lePhone->setText(query.value("phone").toString());
        leEmail->setText(query.value("email").toString());
        leRealname->setText(query.value("realname").toString());
        leIdcard->setText(query.value("idcard").toString());
        dsbBalance->setValue(query.value("balance").toDouble());
    } else {
        QMessageBox::warning(dialog, "错误", "加载用户数据失败！");
        dialog->close();
        return nullptr;
    }

    // 按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("保存");
    QPushButton *btnCancel = new QPushButton("取消");
    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    // 保存按钮点击事件
    connect(btnSave, &QPushButton::clicked, this, [=]() {
        QMap<QString, QVariant> editData;
        editData["password"] = lePassword->text().trimmed();
        editData["phone"] = lePhone->text().trimmed();
        editData["email"] = leEmail->text().trimmed();
        editData["realname"] = leRealname->text().trimmed();
        editData["idcard"] = leIdcard->text().trimmed();
        editData["balance"] = dsbBalance->value();

        // 验证必要字段
        if (editData["password"].toString().isEmpty() ||
            editData["realname"].toString().isEmpty() ||
            editData["idcard"].toString().isEmpty()) {
            QMessageBox::warning(dialog, "提示", "密码、真实姓名和身份证号不能为空！");
            return;
        }

        if (saveUserEditData(username, editData)) {
            QMessageBox::information(dialog, "成功", "用户信息修改成功！");
            dialog->accept();
            loadUserData(); // 刷新用户列表
        } else {
            QMessageBox::warning(dialog, "错误", "修改用户信息失败！");
        }
    });

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    return dialog;
}

// 保存用户编辑数据
bool AdminMainWindow::saveUserEditData(const QString &username, const QMap<QString, QVariant> &editData) {
    QString sql = QString("update user_info set password = '%1', phone = '%2', email = '%3', realname = '%4', idcard = '%5', balance = %6 where username = '%7'").arg(editData["password"].toString().replace("'", "''"), editData["phone"].toString().replace("'", "''"), editData["email"].toString().replace("'", "''"), editData["realname"].toString().replace("'", "''"), editData["idcard"].toString().replace("'", "''"), QString::number(editData["balance"].toDouble()), username);

    bool success;
    dbOperator->DBGetData(sql, success);
    return success;
}

// 重置查询条件并显示所有航班
void AdminMainWindow::on_resetBtn_clicked() {
    ui->departureCbox->setCurrentIndex(0);
    ui->arrivalCbox->setCurrentIndex(0);
    ui->yearSpin->setValue(2026);
    ui->monthSpin->setValue(1);
    ui->daySpin->setValue(1);
    loadFlightData();
    resetStatus=true;
}
