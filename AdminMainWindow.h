#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include <QDate>
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
    void on_flightManagementBtn_clicked();
    void on_orderViewBtn_clicked();
    void on_userManagementBtn_clicked();
    void on_dataStatisticsBtn_clicked();
    void on_backBtn_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_editBtn_clicked();
    void on_deleteBtn_clicked();
    void on_comboBox_2_currentIndexChanged(int index);
    void on_comboBox_3_currentIndexChanged(int index);

private:
    Ui::AdminMainWindow *ui;
    DBOperator *dbOperator;
    QSqlQueryModel *flightModel;
    QSqlQueryModel *orderModel;
    QSqlQueryModel *userModel;

    void initFlightManagement();
    void initOrderView();
    void initUserManagement();
    void initDataStatistics();
    void loadFlightData(const QString &whereClause = "");
    void loadOrderData(const QString &status = "");
    void loadUserData();
    void updateStatistics();
};

#endif // ADMINMAINWINDOW_H
