#include "FlightItemWidget.h"
#include "ui_FlightItemWidget.h"

// 默认构造：只初始化UI (由 Qt Designer 自动生成的逻辑)
FlightItemWidget::FlightItemWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FlightItemWidget) {
    ui->setupUi(this);
}

// 带参数构造：先初始化 UI，再填充数据
FlightItemWidget::FlightItemWidget(const QString& flightNo, const QString& takeoffTime, const QString& departure, const QString& destination, const QString& price, const QString& remaining, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FlightItemWidget)
    , m_flightNo(flightNo) // 保存航班号
{
    ui->setupUi(this); // 先初始化 UI 控件

    // 1. 填充图标 (假设资源文件中有flight_icon.png)
    // ui->iconLabel->setPixmap(QPixmap(":/source/figures/capitalLogo.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 2. 填充文本信息
    ui->flightNoLabel->setText("航班号：" + flightNo);
    ui->takeoffTimeLabel->setText("起飞时间：" + takeoffTime);
    ui->departureLabel->setText("出发地：" + departure);
    ui->destinationLabel->setText("目的地：" + destination);
    ui->priceLabel->setText("票价：" + price);
    ui->remainingLabel->setText("剩余座位：" + remaining);

    // 3. 连接按钮信号 (使用保存的航班号发射信号)
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
