#include "FlightItemWidget.h"
#include "ui_FlightItemWidget.h"

#include <QPixmap>

// 默认构造函数
FlightItemWidget::FlightItemWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FlightItemWidget) {
    ui->setupUi(this);
}

// 带参数构造函数
FlightItemWidget::FlightItemWidget(const QString &flightNo, const QString &takeoffTime, const QString &arriveTime, const QString &departure, const QString &destination, const QString &price, const QString &remaining, const QString &airlineCompany, QWidget *parent): QWidget(parent), ui(new Ui::FlightItemWidget), m_flightNo(flightNo) {
    ui->setupUi(this);

    ui->flightNoLabel->setText(flightNo);
    ui->takeoffTimeLabel->setText(takeoffTime);
    ui->arriveTimeLabel->setText(arriveTime);
    ui->departureLabel->setText(departure);
    ui->destinationLabel->setText(destination);
    ui->priceLabel->setText(price);
    ui->remainingLabel->setText(remaining);

    QString logoPath;
    if (airlineCompany == "中国国航") {
        logoPath = ":/img/source/figures/AirChina.png";
    } else if (airlineCompany == "东方航空") {
        logoPath = ":/img/source/figures/ChinaEasternAirlines.png";
    } else if (airlineCompany == "中国南方航空") {
        logoPath = ":/img/source/figures/ChinaSouthernAirlines.png";
    } else if (airlineCompany == "深圳航空") {
        logoPath = ":/img/source/figures/ShenzhenAirlines.png";
    } else if (airlineCompany == "海南航空") {
    logoPath = ":/img/source/figures/HainanAirlines.png";
    } else if (airlineCompany == "厦门航空") {
        logoPath = ":/img/source/figures/XiamenAir.png";
    } else if (airlineCompany == "四川航空") {
        logoPath = ":/img/source/figures/SichuanAirlines.png";
    } else if (airlineCompany == "吉祥航空") {
        logoPath = ":/img/source/figures/JuneyaoAir.png";
    } else if (airlineCompany == "春秋航空") {
        logoPath = ":/img/source/figures/9C.png";
    } else if (airlineCompany == "山东航空") {
        logoPath = ":/img/source/figures/SC.png";
    } else if (airlineCompany == "上海航空") {
        logoPath = ":/img/source/figures/FM.png";
    } else if (airlineCompany == "首都航空") {
        logoPath = ":/img/source/figures/JD.png";
    } else if (airlineCompany == "祥鹏航空") {
        logoPath = ":/img/source/figures/8L.png";
    } else if (airlineCompany == "中国联合航空公司") {
        logoPath = ":/img/source/figures/KN.png";
    } else {
        logoPath = ":/img/source/figures/AirChina.png";
    }
    ui->airlineLogoLabel->setPixmap(QPixmap(logoPath));

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
