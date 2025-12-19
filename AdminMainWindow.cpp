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
// 新增：编辑对话框需要的头文件
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QMap>

// 构造函数：初始化管理员主窗口
AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AdminMainWindow) {
    ui->setupUi(this);
    dbOperator = new DBOperator();
    dbOperator->DBOpen();

    // ========== 新增：初始化图表指针为 nullptr（避免析构空指针） ==========
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
    // ========== 新增：释放图表相关对象 ==========
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
        // 表头居中（可选，和用户表格保持一致）
        "QHeaderView::section {"
        "    text-align: center;"
        "}"
        );
    // 表头文本居中对齐（可选）
    QHeaderView *flightHeader = ui->flightTable->horizontalHeader();
    if (flightHeader) {
        flightHeader->setDefaultAlignment(Qt::AlignCenter);
    }
    // 列宽自适应内容（可选）
    ui->flightTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    loadFlightData();
}

// 初始化订单查看模块
void AdminMainWindow::initOrderView() {
    orderModel = new QSqlQueryModel(this);

    // ========== 新增：初始化订单表格（twOrderList） ==========
    // 1. 表格选择行为（整行选中）
    ui->twOrderList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twOrderList->setSelectionMode(QAbstractItemView::SingleSelection);
    // 2. 表格样式（和用户表格统一）
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
    // 3. 设置表头和列数（匹配订单表字段）
    ui->twOrderList->setColumnCount(12); // 12个核心字段（无操作列）
    QStringList orderHeaders = {
        "订单号", "用户ID", "航班号", "乘客姓名", "身份证号",
        "出发城市", "起飞时间", "到达城市", "到达时间", "票价",
        "下单时间", "订单状态"
    };
    ui->twOrderList->setHorizontalHeaderLabels(orderHeaders);
    // 4. 表头居中+列宽自适应
    QHeaderView *orderHeader = ui->twOrderList->horizontalHeader();
    if (orderHeader) {
        orderHeader->setDefaultAlignment(Qt::AlignCenter);
        orderHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    // ========== 新增：初始化订单状态筛选下拉框 ==========
    ui->cbOrderStatus->clear();
    ui->cbOrderStatus->addItem("全部订单"); // 替换原有“状态”，更友好
    ui->cbOrderStatus->addItem("已支付");
    ui->cbOrderStatus->addItem("已取消");

    // 加载订单数据
    loadOrderData();
}

// 初始化用户管理模块
void AdminMainWindow::initUserManagement() {
    userModel = new QSqlQueryModel(this);
    
    // 设置表格选择行为：整行选择
    ui->twUserList->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置选择模式：单选
    ui->twUserList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 设置选中行的样式：添加阴影效果和背景高亮
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

//  数据统计页面初始化（创建图表布局和容器）
void AdminMainWindow::initDataStatistics() {
    // ========== 1. 获取 StackedWidget 第3页（数据统计页） ==========
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

    // ========== 2. 创建 QScrollArea 并铺满整个页面 ==========
    QScrollArea *scrollArea = new QScrollArea(statsPage);
    scrollArea->setWidgetResizable(true); // 关键：让内部Widget自适应ScrollArea
    scrollArea->setStyleSheet("QScrollArea { border: none; }"); // 去掉边框更美观

    // ========== 3. 创建 ScrollArea 的内部容器Widget ==========
    QWidget *scrollContent = new QWidget(scrollArea);
    scrollArea->setWidget(scrollContent);

    // ========== 4. 垂直布局：容纳所有大尺寸图表 ==========
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setSpacing(40); // 图表间大间距，更清晰
    mainLayout->setContentsMargins(30, 30, 30, 30); // 内边距

    // ========== 5. 初始化3个大尺寸图表（宽度铺满，高度400px） ==========
    // 5.1 核心指标柱状图
    QLabel *totalTitle = new QLabel("核心运营数据总览", scrollContent);
    totalTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(totalTitle, 0, Qt::AlignCenter);

    totalStatsChart = new QChart();
    totalStatsChart->setTitle("总航班数 / 总用户数 / 总订单数 / 订单总金额");
    totalStatsChart->setAnimationOptions(QChart::SeriesAnimations);
    totalStatsView = new QChartView(totalStatsChart);
    totalStatsView->setRenderHint(QPainter::Antialiasing);
    totalStatsView->setMinimumHeight(400); // 大尺寸高度
    totalStatsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 宽度铺满，高度固定
    totalStatsView->setStyleSheet("border: 1px solid #ecf0f1; border-radius: 10px; padding: 10px;");
    mainLayout->addWidget(totalStatsView);

    // 5.2 航空公司航班数饼状图
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

    // ========== 6. 将 ScrollArea 铺满整个 StackedWidget 页面 ==========
    QVBoxLayout *pageLayout = new QVBoxLayout(statsPage);
    pageLayout->setContentsMargins(0, 0, 0, 0); // 无内边距，完全铺满
    pageLayout->addWidget(scrollArea);

    // ========== 7. 初始化图表数据 ==========
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

            // connect(editBtn, &QPushButton::clicked, [this, flightId]() {
            //     Q_UNUSED(flightId);
            // });
            connect(editBtn, &QPushButton::clicked, this, [this, flightId]() {
                onEditFlight(flightId); // 调用编辑航班的槽函数
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

// 加载订单数据到 twOrderList 表格
void AdminMainWindow::loadOrderData(const QString &status) {
    // 1. 构造SQL：查询订单表所有字段（匹配表头顺序）
    QString sql = "select "
                  "order_id, user_id, flight_id, passenger_name, passenger_idcard, "
                  "departure_city, departure_time, arrival_city, arrival_time, price, "
                  "order_time, order_status "
                  "from order_info";

    // 2. 按状态筛选（处理“全部订单”/“已支付”/“已取消”）
    QString filterStatus = status.trimmed();
    if (!filterStatus.isEmpty() && filterStatus != "全部订单") {
        // 转义单引号，避免SQL语法错误
        sql += " where order_status = '" + filterStatus.replace("'", "''") + "'";
    }

    // 3. 执行SQL查询
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        // 4. 清空表格（关键：清空twOrderList而非twUserList）
        ui->twOrderList->setRowCount(0);

        // 5. 遍历结果填充表格
        int row = 0;
        while (query.next()) {
            ui->twOrderList->insertRow(row);

            // 遍历12个字段，填充到对应列
            for (int col = 0; col < 12; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                // 单元格文本居中（和用户表格统一）
                item->setTextAlignment(Qt::AlignCenter);
                ui->twOrderList->setItem(row, col, item);
            }

            row++;
        }

        // 6. 列宽自适应
        ui->twOrderList->resizeColumnsToContents();
    } else {
        // 7. 错误提示（包含具体错误信息）
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

        // 填充用户数据到表格
        int row = 0;
        while (query.next()) {
            ui->twUserList->insertRow(row);

            for (int col = 0; col < 7; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                // 设置单元格文本居中对齐
                item->setTextAlignment(Qt::AlignCenter);
                ui->twUserList->setItem(row, col, item);
            }

            row++;
        }

        // 设置表头文本居中对齐
        QHeaderView *header = ui->twUserList->horizontalHeader();
        if (header) {
            header->setDefaultAlignment(Qt::AlignCenter);
        }

        // 根据内容自动调整列宽
        ui->twUserList->resizeColumnsToContents();
        
        // 设置列宽调整模式：根据内容调整，但保留最小宽度
        ui->twUserList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        QMessageBox::warning(this, "错误", "加载用户数据失败");
    }
}

// 更新统计数据
void AdminMainWindow::updateStatistics() {
    // ---------------- 1. 核心指标：总航班/用户/订单/金额（柱状图） ----------------
    qint64 totalFlights = 0;    // 总航班数
    qint64 totalUsers = 0;      // 总用户数
    qint64 totalOrders = 0;     // 总订单数
    double totalAmount = 0.0;   // 订单总金额

    // ===== 调试：打印数据库查询结果 =====
    qDebug() << "===== 核心指标查询 =====";
    bool success;

    // 查询总航班数
    QSqlQuery flightQuery = dbOperator->DBGetData("SELECT COUNT(*) AS flight_count FROM flight_info", success);
    if (success && flightQuery.next()) {
        totalFlights = flightQuery.value("flight_count").toLongLong();
        qDebug() << "总航班数：" << totalFlights;
    } else {
        qDebug() << "总航班数查询失败：" << flightQuery.lastError().text();
    }

    // 查询总用户数
    QSqlQuery userQuery = dbOperator->DBGetData("SELECT COUNT(*) AS user_count FROM user_info", success);
    if (success && userQuery.next()) {
        totalUsers = userQuery.value("user_count").toLongLong();
        qDebug() << "总用户数：" << totalUsers;
    } else {
        qDebug() << "总用户数查询失败：" << userQuery.lastError().text();
    }

    // 查询总订单数 + 总金额
    QSqlQuery orderQuery = dbOperator->DBGetData("SELECT COUNT(*) AS order_count, COALESCE(SUM(price), 0) AS total_price FROM order_info", success);
    if (success && orderQuery.next()) {
        totalOrders = orderQuery.value("order_count").toLongLong();
        totalAmount = orderQuery.value("total_price").toDouble();
        qDebug() << "总订单数：" << totalOrders << " 总金额：" << totalAmount;
    } else {
        qDebug() << "总订单数查询失败：" << orderQuery.lastError().text();
    }

    // ========== 终极极简版：Qt5全兼容 + 无任何编译报错 ==========
    totalStatsChart->removeAllSeries(); // 清空所有系列

    // 遍历移除所有旧轴（Qt5/6通用）
    QList<QAbstractAxis*> oldAxes = totalStatsChart->axes();
    for (QAbstractAxis* axis : oldAxes) {
        totalStatsChart->removeAxis(axis);
    }

    // 1. 创建数据Set（仅存数据+柱子颜色）
    QBarSet *countSet = new QBarSet("数量");
    QBarSet *amountSet = new QBarSet("金额");
    // 填充数据
    *countSet << totalFlights << totalUsers << totalOrders << 0;
    *amountSet << 0 << 0 << 0 << totalAmount;
    // 仅保留柱子颜色（Qt5肯定支持）
    countSet->setColor(QColor(52, 152, 219));     // 数量柱：蓝色
    amountSet->setColor(QColor(231, 76, 60));     // 金额柱：红色

    // 2. 创建Series并设置【仅保留Qt5兼容的标签接口】
    QBarSeries *countSeries = new QBarSeries();
    QBarSeries *amountSeries = new QBarSeries();
    countSeries->append(countSet);
    amountSeries->append(amountSet);

    // ---------------- 数量系列：仅保留3个Qt5兼容接口 ----------------
    countSeries->setLabelsVisible(true);          // 显示标签（Qt5支持）
    countSeries->setLabelsFormat("%lld");         // 正整数格式（Qt5支持）
    countSeries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd); // 顶端外侧（Qt5支持）

    // ---------------- 金额系列：仅保留3个Qt5兼容接口 ----------------
    amountSeries->setLabelsVisible(true);
    amountSeries->setLabelsFormat("%.0f");        // 金额无小数
    amountSeries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);

    // 3. 原始横坐标（无过长问题）
    QBarCategoryAxis *totalXAxis = new QBarCategoryAxis();
    totalXAxis->append({"总航班数", "总用户数", "总订单数", "订单总金额(元)"});
    totalXAxis->setLabelsFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->addAxis(totalXAxis, Qt::AlignBottom);
    countSeries->attachAxis(totalXAxis);
    amountSeries->attachAxis(totalXAxis);

    // 4. 数量轴：正整数 + 不冲顶（1.3倍范围）
    QValueAxis *axisY_Count = new QValueAxis();
    axisY_Count->setTitleText("数量");
    axisY_Count->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Count->setLabelFormat("%d");          // 仅显示正整数（Qt5支持）
    axisY_Count->setTickType(QValueAxis::TicksFixed);
    axisY_Count->setTickInterval(1);            // 刻度间隔1（Qt5支持）
    qint64 maxCount = qMax(totalFlights, qMax(totalUsers, totalOrders));
    int countRangeMax = (maxCount <= 0) ? 10 : static_cast<int>(maxCount * 1.3);
    axisY_Count->setRange(0, countRangeMax);
    totalStatsChart->addAxis(axisY_Count, Qt::AlignLeft);
    countSeries->attachAxis(axisY_Count);

    // 5. 金额轴：整数 + 不冲顶（1.3倍范围）
    QValueAxis *axisY_Amount = new QValueAxis();
    axisY_Amount->setTitleText("金额（元）");
    axisY_Amount->setLabelsFont(QFont("Microsoft YaHei", 12));
    axisY_Amount->setLabelFormat("%.0f");        // 无小数（Qt5支持）
    axisY_Amount->setTickType(QValueAxis::TicksFixed);
    axisY_Amount->setTickInterval(100);
    double amountRangeMax = (totalAmount <= 0) ? 1000 : totalAmount * 1.3;
    axisY_Amount->setRange(0, amountRangeMax);
    totalStatsChart->addAxis(axisY_Amount, Qt::AlignRight);
    amountSeries->attachAxis(axisY_Amount);

    // 6. 最终设置 + 加大顶部边距（确保标签显示）
    totalStatsChart->addSeries(countSeries);
    totalStatsChart->addSeries(amountSeries);
    totalStatsChart->setTitle("核心运营数据总览（数量/金额双轴）");
    totalStatsChart->setAnimationOptions(QChart::SeriesAnimations);
    totalStatsChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    totalStatsChart->legend()->setAlignment(Qt::AlignBottom);
    // 顶部边距加大，确保顶端标签不被截断
    totalStatsChart->setMargins(QMargins(20, 40, 20, 20));

    // ========== 饼图部分代码保持不变 ==========
    QList<QColor> pieColors = {QColor(46, 204, 113), QColor(155, 89, 182), QColor(231, 76, 60),
                               QColor(241, 196, 15), QColor(52, 152, 219), QColor(127, 140, 141)};

    // ---------------- 2. 各航空公司航班数：饼状图 ----------------
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
            colorIdx++;
        }
    } else {
        qDebug() << "航空公司航班数查询失败：" << airFlightQuery.lastError().text();
    }

    airlineFlightChart->addSeries(airlinePieSeries);
    airlineFlightChart->setTitle("各航空公司航班数量占比（百分比）");
    airlineFlightChart->setTitleFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    airlineFlightChart->legend()->setFont(QFont("Microsoft YaHei", 12));
    airlineFlightChart->legend()->setAlignment(Qt::AlignRight);
    airlineFlightChart->createDefaultAxes();

    // ---------------- 3. 各城市出发航班数：饼状图 ----------------
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
            colorIdx++;
        }
    } else {
        qDebug() << "城市航班数查询失败：" << cityFlightQuery.lastError().text();
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

// 处理用户操作下拉框选择变化 (退出登录或修改密码)
void AdminMainWindow::on_comboBox_currentIndexChanged(int index) {
    if (index == 2) {
        emit logoutRequested();
        this->close();
    } else if (index == 1) {
        QMessageBox::information(this, "提示", "修改密码功能待实现");
    }
}

// 编辑选中的航班（顶部编辑按钮）
void AdminMainWindow::on_editBtn_clicked() {
    int row = ui->flightTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的航班");
        return;
    }

    // 获取选中行的航班ID
    QString flightId = ui->flightTable->item(row, 0)->text();
    // 调用编辑函数
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
    // 获取选中的状态（如“全部订单”/“已支付”/“已取消”）
    QString status = ui->cbOrderStatus->currentText();
    // 刷新订单数据
    loadOrderData(status);
}

// 处理删除用户按钮点击
void AdminMainWindow::on_deleteUserBtn_clicked() {
    // 检查是否选中了用户
    int row = ui->twUserList->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的用户");
        return;
    }

    // 获取选中行的用户名
    QString username = ui->twUserList->item(row, 0)->text();

    // 显示确认对话框
    int ret = QMessageBox::question(this, "确认删除", QString("确定要删除用户 %1 吗？\n此操作不可恢复！").arg(username), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    // 执行删除操作
    QString sql = QString("delete from user_info where username = '%1'").arg(username);
    bool success;
    QSqlQuery query = dbOperator->DBGetData(sql, success);

    if (success) {
        // 删除成功，刷新用户列表
        loadUserData();
        QMessageBox::information(this, "删除成功", QString("用户 %1 已成功删除").arg(username));
    } else {
        // 删除失败，显示错误信息
        QMessageBox::warning(this, "删除失败", "删除用户失败：" + query.lastError().text());
    }
}

void AdminMainWindow::on_findBtn_clicked()
{
    // 1. 获取筛选条件（去除首尾空格，避免无效匹配）
    QString departureCity = ui->departureLineEdit->text().trimmed();  // 出发地输入框（请根据实际UI控件名修改）
    QString arrivalCity = ui->destinationLineEdit->text().trimmed(); // 目的地输入框（请根据实际UI控件名修改）
    int year = ui->yearSpin->value();
    int month = ui->monthSpin->value();
    int day = ui->daySpin->value();

    // 2. 构建筛选条件（空条件不参与过滤）
    QStringList conditions;

    // 出发地条件
    if (!departureCity.isEmpty()) {
        conditions.append(QString("departure_city = '%1'").arg(departureCity));
    }

    // 目的地条件
    if (!arrivalCity.isEmpty()) {
        conditions.append(QString("arrival_city = '%1'").arg(arrivalCity));
    }

    // 日期条件（验证日期合法性，避免2月30日等无效日期）
    QDate selectedDate(year, month, day);
    if (selectedDate.isValid()) {
        QString dateStr = selectedDate.toString("yyyy-MM-dd");
        // 筛选出发时间的日期部分等于所选日期（兼容MySQL/SQLite的DATE函数）
        conditions.append(QString("DATE(departure_time) = '%1'").arg(dateStr));
    }

    // 3. 拼接WHERE子句（多条件用AND连接）
    QString whereClause = conditions.join(" AND ");

    // 4. 加载筛选后的航班数据（内部会自动清空表格并重新填充）
    loadFlightData(whereClause);

    // 5. 无匹配数据时给出友好提示
    if (ui->flightTable->rowCount() == 0) {
        QMessageBox::information(this, "查询结果", "未找到符合筛选条件的航班信息！");
    }
}

// 创建航班编辑对话框
QDialog *AdminMainWindow::createFlightEditDialog(const QString &flightId) {
    // 1. 创建模态对话框
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("编辑航班信息");
    dialog->setModal(true); // 模态：阻塞父窗口操作
    dialog->setFixedSize(600, 450); // 固定窗口大小，避免变形

    // 2. 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QGridLayout *formLayout = new QGridLayout(); // 表单布局：标签+输入框
    mainLayout->addLayout(formLayout);

    // 3. 初始化输入控件（按航班字段逐一创建）
    // 3.1 航班号（不可编辑，仅展示）
    QLabel *lblFlightId = new QLabel("航班号：");
    QLineEdit *leFlightId = new QLineEdit();
    leFlightId->setReadOnly(true); // 航班号是主键，禁止修改
    formLayout->addWidget(lblFlightId, 0, 0);
    formLayout->addWidget(leFlightId, 0, 1);

    // 3.2 航空公司
    QLabel *lblAirline = new QLabel("航空公司：");
    QLineEdit *leAirline = new QLineEdit();
    formLayout->addWidget(lblAirline, 1, 0);
    formLayout->addWidget(leAirline, 1, 1);

    // 3.3 出发城市
    QLabel *lblDepartCity = new QLabel("出发城市：");
    QLineEdit *leDepartCity = new QLineEdit();
    formLayout->addWidget(lblDepartCity, 2, 0);
    formLayout->addWidget(leDepartCity, 2, 1);

    // 3.4 出发机场
    QLabel *lblDepartAirport = new QLabel("出发机场：");
    QLineEdit *leDepartAirport = new QLineEdit();
    formLayout->addWidget(lblDepartAirport, 3, 0);
    formLayout->addWidget(leDepartAirport, 3, 1);

    // 3.5 出发时间
    QLabel *lblDepartTime = new QLabel("出发时间：");
    QDateTimeEdit *dtEditDepart = new QDateTimeEdit();
    dtEditDepart->setDisplayFormat("yyyy-MM-dd hh:mm");
    dtEditDepart->setCalendarPopup(true); // 弹出日历选择
    formLayout->addWidget(lblDepartTime, 4, 0);
    formLayout->addWidget(dtEditDepart, 4, 1);

    // 3.6 到达城市
    QLabel *lblArriveCity = new QLabel("到达城市：");
    QLineEdit *leArriveCity = new QLineEdit();
    formLayout->addWidget(lblArriveCity, 5, 0);
    formLayout->addWidget(leArriveCity, 5, 1);

    // 3.7 到达机场
    QLabel *lblArriveAirport = new QLabel("到达机场：");
    QLineEdit *leArriveAirport = new QLineEdit();
    formLayout->addWidget(lblArriveAirport, 6, 0);
    formLayout->addWidget(leArriveAirport, 6, 1);

    // 3.8 到达时间
    QLabel *lblArriveTime = new QLabel("到达时间：");
    QDateTimeEdit *dtEditArrive = new QDateTimeEdit();
    dtEditArrive->setDisplayFormat("yyyy-MM-dd hh:mm");
    dtEditArrive->setCalendarPopup(true);
    formLayout->addWidget(lblArriveTime, 7, 0);
    formLayout->addWidget(dtEditArrive, 7, 1);

    // 3.9 价格
    QLabel *lblPrice = new QLabel("价格（元）：");
    QDoubleSpinBox *dsbPrice = new QDoubleSpinBox();
    dsbPrice->setRange(0.0, 99999.99); // 价格范围限制
    dsbPrice->setDecimals(2); // 保留2位小数
    formLayout->addWidget(lblPrice, 8, 0);
    formLayout->addWidget(dsbPrice, 8, 1);

    // 3.10 总座位数
    QLabel *lblTotalSeats = new QLabel("总座位数：");
    QSpinBox *sbTotalSeats = new QSpinBox();
    sbTotalSeats->setRange(1, 999); // 至少1个座位
    formLayout->addWidget(lblTotalSeats, 9, 0);
    formLayout->addWidget(sbTotalSeats, 9, 1);

    // 3.11 剩余座位数
    QLabel *lblRemainSeats = new QLabel("剩余座位数：");
    QSpinBox *sbRemainSeats = new QSpinBox();
    sbRemainSeats->setRange(0, 999); // 可以为0
    formLayout->addWidget(lblRemainSeats, 10, 0);
    formLayout->addWidget(sbRemainSeats, 10, 1);

    // 4. 加载该航班的原始数据到输入框
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

    // 5. 创建按钮布局（保存+取消）
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("保存");
    QPushButton *btnCancel = new QPushButton("取消");
    btnLayout->addStretch(); // 按钮右对齐
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    // 6. 绑定按钮事件
    // 保存按钮：收集输入数据并保存
    connect(btnSave, &QPushButton::clicked, this, [=]() {
        // 收集编辑后的数据
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

        // 数据校验
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

        // 保存数据
        if (saveFlightEditData(flightId, editData)) {
            QMessageBox::information(dialog, "成功", "航班信息修改成功！");
            dialog->accept(); // 关闭对话框并标记为“确认”
            loadFlightData(); // 刷新航班表格
        } else {
            QMessageBox::warning(dialog, "错误", "修改航班信息失败！");
        }
    });

    // 取消按钮：关闭对话框，不保存
    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    return dialog;
}

// 保存编辑后的航班数据到数据库
bool AdminMainWindow::saveFlightEditData(const QString &flightId, const QMap<QString, QVariant> &editData) {
    // 拼接UPDATE SQL（转义单引号，避免SQL语法错误）
    QString sql = QString(
                      "UPDATE flight_info SET "
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
                      "WHERE flight_id = '%11'"
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

    // 执行SQL并返回结果
    bool success;
    dbOperator->DBGetData(sql, success);
    return success;
}

// 编辑航班的核心槽函数（接收航班ID，弹出编辑对话框）
void AdminMainWindow::onEditFlight(const QString &flightId) {
    // 创建并显示编辑对话框
    QDialog *editDialog = createFlightEditDialog(flightId);
    if (editDialog) {
        editDialog->exec(); // 显示模态对话框
        delete editDialog; // 对话框关闭后释放内存
    }
}
