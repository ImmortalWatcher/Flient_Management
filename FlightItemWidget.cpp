#include "FlightItemWidget.h"
#include "ui_FlightItemWidget.h"

#include <QPixmap>

// 默认构造函数：仅初始化 UI
FlightItemWidget::FlightItemWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FlightItemWidget) {
    ui->setupUi(this);
}

// 带参数构造函数：初始化 UI 并填充航班数据
FlightItemWidget::FlightItemWidget(const QString &flightNo, const QString &takeoffTime, const QString &arriveTime, const QString &departure, const QString &destination, const QString &price, const QString &remaining, const QString &airlineCompany, QWidget *parent): QWidget(parent), ui(new Ui::FlightItemWidget), m_flightNo(flightNo) {
    ui->setupUi(this);

    // 填充航班信息到界面
    ui->flightNoLabel->setText(flightNo);
    ui->takeoffTimeLabel->setText(takeoffTime);
    ui->arriveTimeLabel->setText(arriveTime);
    ui->departureLabel->setText(departure);
    ui->destinationLabel->setText(destination);
    ui->priceLabel->setText(price);
    ui->remainingLabel->setText(remaining);

    // 根据航空公司名称设置对应的logo
    QString logoPath;
    if (airlineCompany == "中国国航") {
        logoPath = ":/img/source/figures/AirChina.png";
    } else if (airlineCompany == "东方航空") {
        logoPath = ":/img/source/figures/ChinaEasternAirlines.png";
    } else if (airlineCompany == "中国南方航空") {
        logoPath = ":/img/source/figures/ChinaSouthernAirlines.png";
    } else if (airlineCompany == "深圳航空") {
        logoPath = ":/img/source/figures/ShenzhenAirlines.png";
    } else {
        // 默认使用中国国航 logo
        logoPath = ":/img/source/figures/AirChina.png";
    }
    ui->airlineLogoLabel->setPixmap(QPixmap(logoPath));

    // 连接预订和收藏按钮的信号
    connect(ui->bookBtn, &QPushButton::clicked, this, [=]() {
        emit bookClicked(m_flightNo);
    });
    connect(ui->collectBtn, &QPushButton::clicked, this, [=]() {
        emit collectClicked(m_flightNo);
    });
}

FlightItemWidget::~FlightItemWidget() {
    delete ui;
}
