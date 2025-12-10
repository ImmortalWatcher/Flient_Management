#ifndef FLIGHTITEMWIDGET_H
#define FLIGHTITEMWIDGET_H

#include <QWidget>

namespace Ui { class FlightItemWidget; }

class FlightItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit FlightItemWidget(QWidget *parent = nullptr);
    ~FlightItemWidget();
    explicit FlightItemWidget(const QString& flightNo, const QString& takeoffTime, const QString& arriveTime, const QString& departure, const QString& destination, const QString& price, const QString& remaining, const QString& airlineCompany, QWidget *parent = nullptr);

private:
    Ui::FlightItemWidget *ui;
    QString m_flightNo;

signals:
    void bookClicked(const QString& flightNo);
    void collectClicked(const QString& flightNo);
};

#endif // FLIGHTITEMWIDGET_H
