#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QDate>
#include <QMainWindow>
#include <QSqlQueryModel>

#include "DBOperator.h"

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
    void on_cbUserOperation_currentIndexChanged(int index);

    void on_findBtn_clicked();

private:
    // UI 相关
    Ui::AdminMainWindow *ui;

    // 数据相关
    DBOperator *dbOperator;
    QSqlQueryModel *flightModel;
    QSqlQueryModel *orderModel;
    QSqlQueryModel *userModel;

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
};

#endif // ADMINMAINWINDOW_H
