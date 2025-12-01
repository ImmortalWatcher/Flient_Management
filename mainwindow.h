#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "LoginDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(LoginDlg *loginDlg = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

private:
    Ui::MainWindow *ui;
    LoginDlg *m_loginDlg;
};
#endif // MAINWINDOW_H
