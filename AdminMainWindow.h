#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QDate>
#include <QMainWindow>
#include <QSqlQueryModel>
#include <QPainter>
#include <QComboBox>

#define QT_CHARTS_USE_NAMESPACE

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
    void on_deleteUserBtn_2_clicked();

    void on_deleteUserBtn_clicked();

    void on_findBtn_clicked();

    void onEditFlight(const QString &flightId);

    void on_resetBtn_clicked();

private:
    Ui::AdminMainWindow *ui;
    DBOperator *dbOperator;
    QSqlQueryModel *flightModel;
    QSqlQueryModel *orderModel;
    QSqlQueryModel *userModel;

    QChart *totalStatsChart;
    QChart *airlineFlightChart;
    QChart *airlineOrderChart;
    QChart *cityFlightChart;
    QChart *cityOrderChart;
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

    void loadFlightData(const QString &whereClause = "");
    void loadOrderData(const QString &status = "");
    void loadUserData();
    void updateStatistics();

    QDialog *createFlightEditDialog(const QString &flightId);
    bool saveFlightEditData(const QString &flightId, const QMap<QString, QVariant> &editData);
    QDialog *createUserEditDialog(const QString &username);
    bool saveUserEditData(const QString &username, const QMap<QString, QVariant> &editData);
    void fillComboBox(QComboBox *cbox, const QString &sql);
};

#endif // ADMINMAINWINDOW_H
