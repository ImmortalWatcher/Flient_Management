#include "FlightItemWidget.h"
#include "ui_FlightItemWidget.h"

// 默认构造：只初始化UI（由Qt Designer自动生成的逻辑）
FlightItemWidget::FlightItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FlightItemWidget)
{
    ui->setupUi(this);
}

// 带参数构造：先初始化UI，再填充数据
FlightItemWidget::FlightItemWidget(const QString& flightNo,
                                   const QString& takeoffTime,
                                   const QString& arriveTime,
                                   const QString& departure,
                                   const QString& destination,
                                   const QString& price,
                                   const QString& remaining,
                                   QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FlightItemWidget)
    , m_flightNo(flightNo) // 保存航班号
{
    ui->setupUi(this); // 先初始化UI控件

    // 1. 填充图标（假设资源文件中有flight_icon.png）
    // ui->iconLabel->setPixmap(QPixmap(":/source/figures/capitalLogo.png")
                                 // .scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 2. 填充文本信息
    ui->flightNoLabel->setText(flightNo);
    ui->takeoffTimeLabel->setText(takeoffTime);
    ui->arriveTimeLabel->setText(arriveTime);
    ui->departureLabel->setText(departure);
    ui->destinationLabel->setText(destination);
    ui->priceLabel->setText(price);
    ui->remainingLabel->setText(remaining);

    // 3. 连接按钮信号（使用保存的航班号发射信号）
    connect(ui->bookBtn, &QPushButton::clicked, this, [=]() {
        emit bookClicked(m_flightNo);
    });
    connect(ui->collectBtn, &QPushButton::clicked, this, [=]() {
        emit collectClicked(m_flightNo);
    });
}

FlightItemWidget::~FlightItemWidget()
{
    delete ui;
}
