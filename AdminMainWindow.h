#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QDate>
#include <QMainWindow>
#include <QSqlQueryModel>
#include <QPainter>

#define QT_CHARTS_USE_NAMESPACE
// ========== 新增 Qt Charts 相关头文件 ==========
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSlice>

QT_CHARTS_USE_NAMESPACE

#include "DBOperator.h"

class QDialog;
class QLineEdit;
class QDateTimeEdit;
class QDoubleSpinBox;
class QSpinBox;
class QLabel;
class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;

namespace Ui { class AdminMainWindow; }

class AdminMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow();

signals:
    void logoutRequested();

private slots:
    // 页面切换相关
    void on_flightManagementBtn_clicked();
    void on_orderViewBtn_clicked();
    void on_userManagementBtn_clicked();
    void on_dataStatisticsBtn_clicked();
    void on_backBtn_clicked();

    // 航班管理相关
    void on_editBtn_clicked();
    void on_deleteBtn_clicked();

    // 下拉框选择相关
    void on_comboBox_currentIndexChanged(int index);
    void on_cbOrderStatus_currentIndexChanged(int index);

    // 用户管理相关
    void on_deleteUserBtn_clicked();

    void on_findBtn_clicked();

    void onEditFlight(const QString &flightId); // 新增：编辑航班的核心槽函数

private:
    // UI 相关
    Ui::AdminMainWindow *ui;

    // 数据相关
    DBOperator *dbOperator;
    QSqlQueryModel *flightModel;
    QSqlQueryModel *orderModel;
    QSqlQueryModel *userModel;

    // ========== 新增：图表相关成员变量 ==========
    QChart *totalStatsChart;       // 总统计（航班、用户、订单、金额）
    QChart *airlineFlightChart;    // 各航空公司航班数
    QChart *airlineOrderChart;     // 各航空公司订单数
    QChart *cityFlightChart;       // 各城市航班数
    QChart *cityOrderChart;        // 各城市订单数
    QChartView *totalStatsView;
    QChartView *airlineFlightView;
    QChartView *airlineOrderView;
    QChartView *cityFlightView;
    QChartView *cityOrderView;

    // 初始化相关
    void initFlightManagement();
    void initOrderView();
    void initUserManagement();
    void initDataStatistics();

    // 数据加载相关
    void loadFlightData(const QString &whereClause = "");
    void loadOrderData(const QString &status = "");
    void loadUserData();
    void updateStatistics();

    //：航班编辑对话框相关
    QDialog *createFlightEditDialog(const QString &flightId); // 创建编辑对话框的函数
    bool saveFlightEditData(const QString &flightId, const QMap<QString, QVariant> &editData); // 保存编辑数据的函数
};

#endif // ADMINMAINWINDOW_H
