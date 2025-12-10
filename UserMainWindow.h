#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "DBOperator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserMainWindow; }
QT_END_NAMESPACE

class UserMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit UserMainWindow(int userId = 0, QWidget *parent = nullptr);
    ~UserMainWindow();

signals:
    void logoutRequested();

private slots:
    void on_flightQueryBtn_clicked();
    void on_myOrdersBtn_clicked();
    void on_myFavoritesBtn_clicked();
    void on_personalCenterBtn_clicked();
    void on_backBtn_clicked();

    void on_searchBtn_clicked();
    void on_resetBtn_clicked();
    void on_book_clicked(const QString& flightNo);
    void on_collect_clicked(const QString& flightNo);


private:
    Ui::UserMainWindow *ui;
    int m_userId;
    DBOperator m_dbOperator;
    QVBoxLayout *flightLayout;

    void loadAvatar();
    void showAvatarselectionDialog();
    void setAvatar(int avatarId);
    void loadUserInfo();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void clearFlightItems();
    void loadAllFlights();
};
#endif // USERMAINWINDOW_H
