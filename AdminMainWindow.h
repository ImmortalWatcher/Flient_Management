#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>

namespace Ui { class AdminMainWindow; }

class AdminMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    void on_flightManagementBtn_clicked();
    void on_orderViewBtn_clicked();
    void on_userManagementBtn_clicked();
    void on_dataStatisticsBtn_clicked();
    void on_backBtn_clicked();

private:
    Ui::AdminMainWindow *ui;
};

#endif // ADMINMAINWINDOW_H
