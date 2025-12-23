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
        logoPath = ":/img/source/airlinelogos/CA.png";
    } else if (airlineCompany == "中国联合航空公司") {
        logoPath = ":/img/source/airlinelogos/KN.png";
    } else if (airlineCompany == "东方航空") {
        logoPath = ":/img/source/airlinelogos/MU.png";
    } else if (airlineCompany == "海南航空") {
        logoPath = ":/img/source/airlinelogos/HU.png";
    } else if (airlineCompany == "吉祥航空") {
        logoPath = ":/img/source/airlinelogos/HO.png";
    } else if (airlineCompany == "山东航空") {
        logoPath = ":/img/source/airlinelogos/SC.png";
    } else if (airlineCompany == "上海航空") {
        logoPath = ":/img/source/airlinelogos/FM.png";
    } else if (airlineCompany == "深圳航空") {
        logoPath = ":/img/source/airlinelogos/ZH.png";
    } else if (airlineCompany == "四川航空") {
        logoPath = ":/img/source/airlinelogos/3U.png";
    } else if (airlineCompany == "厦门航空") {
        logoPath = ":/img/source/airlinelogos/MF.png";
    } else if (airlineCompany == "祥鹏航空") {
        logoPath = ":/img/source/airlinelogos/8L.png";
    } else if (airlineCompany == "春秋航空") {
        logoPath = ":/img/source/airlinelogos/9C.png";
    } else if (airlineCompany == "中国南方航空") {
        logoPath = ":/img/source/airlinelogos/CZ.png";
    } else if (airlineCompany == "首都航空") {
        logoPath = ":/img/source/airlinelogos/JD.png";
    } else {
        logoPath = ":/img/source/airlinelogos/CA.png";
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
