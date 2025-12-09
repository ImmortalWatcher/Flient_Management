#include "UserMainWindow.h"
#include "ui_UserMainWindow.h"
#include "FlightItemWidget.h"

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSqlError>
#include <QDateTime>

UserMainWindow::UserMainWindow(int userId, QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::UserMainWindow), m_userId(userId) {
    ui->setupUi(this);

    m_dbOperator.DBOpen();

    // 设置头像标签为可点击
    ui->avatarLabel->setCursor(Qt::PointingHandCursor);
    ui->avatarLabel->installEventFilter(this);

    // 加载用户信息和头像
    loadUserInfo();
    loadAvatar();

    flightLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    ui->scrollAreaWidgetContents->setLayout(flightLayout);
    flightLayout->setSpacing(3); // 条目之间的间距
    flightLayout->setContentsMargins(10, 5, 10, 5);
    flightLayout->setAlignment(Qt::AlignTop); // 确保条目从顶部开始排列
    
    // 移除固定的minimumSize，让内容区域根据内容自动调整大小
    ui->scrollAreaWidgetContents->setMinimumSize(0, 0);
    // 确保scrollAreaWidgetContents有正确的大小策略
    // 当widgetResizable为true时，宽度会自动调整，高度应该根据内容调整
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    // 确保scrollAreaWidgetContents初始可见
    ui->scrollAreaWidgetContents->show();
    
    connect(ui->searchBtn, &QPushButton::clicked, this, &UserMainWindow::on_searchBtn_clicked);
}

UserMainWindow::~UserMainWindow() {
    delete ui;
}

void UserMainWindow::on_flightQueryBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void UserMainWindow::on_myOrdersBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void UserMainWindow::on_myFavoritesBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void UserMainWindow::on_personalCenterBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    loadUserInfo(); // 加载用户信息
}

void UserMainWindow::on_backBtn_clicked() {
    emit logoutRequested();
    this->close();
}

bool UserMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->avatarLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            showAvatarSelectionDialog();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void UserMainWindow::loadAvatar() {
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
        setAvatar(userInfo.avatarId);
    } else {
        // 如果获取失败，使用默认头像
        setAvatar(1);
    }
}

void UserMainWindow::setAvatar(int avatarId) {
    QString avatarPath = QString(":/img/source/avatars/avatar_%1.png").arg(avatarId);
    ui->avatarLabel->setPixmap(QPixmap(avatarPath));
    ui->avatarLabel->setScaledContents(true);
}

void UserMainWindow::showAvatarSelectionDialog() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择头像");
    dialog->setModal(true);
    dialog->resize(550, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    QLabel *titleLabel = new QLabel("请选择您的头像：", dialog);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // 创建网格布局显示所有头像
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(20, 10, 20, 10);

    // 创建一个事件过滤器类来处理点击
    class AvatarClickFilter : public QObject {
    public:
        AvatarClickFilter(int avatarId, int userId, DBOperator *dbOperator, UserMainWindow *parent, QDialog *dialog) : QObject(parent), m_avatarId(avatarId), m_userId(userId), m_dbOperator(dbOperator), m_parent(parent), m_dialog(dialog) {}

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    if (m_userId > 0) {
                        bool success = m_dbOperator->updateUserAvatarId(m_userId, m_avatarId);
                        if (success) {
                            m_parent->setAvatar(m_avatarId);
                            QMessageBox::information(m_parent, "成功", "头像已更新！");
                            m_dialog->accept();
                        } else {
                            QMessageBox::warning(m_parent, "错误", "头像更新失败！");
                        }
                    }
                    return true;
                }
            }
            return QObject::eventFilter(obj, event);
        }

    private:
        int m_avatarId;
        int m_userId;
        DBOperator *m_dbOperator;
        UserMainWindow *m_parent;
        QDialog *m_dialog;
    };

    for (int i = 1; i <= 15; i++) {
        // 构建头像资源路径
        QString avatarPath = QString(":/img/source/avatars/avatar_%1.png").arg(i);

        // 创建头像标签
        QLabel *avatarLabel = new QLabel(dialog);
        avatarLabel->setPixmap(QPixmap(avatarPath));
        avatarLabel->setScaledContents(true);
        avatarLabel->setAlignment(Qt::AlignCenter);
        avatarLabel->setStyleSheet(
            "QLabel {"
            "    border: 2px solid transparent;"
            "    padding: 5px;"
            "}"
            "QLabel:hover {"
            "    border: 2px solid #156080;"
            "    background-color: rgba(21, 96, 128, 0.1);"
            "}"
        );
        avatarLabel->setCursor(Qt::PointingHandCursor);
        avatarLabel->setFixedSize(90, 90);

        // 安装事件过滤器
        avatarLabel->installEventFilter(new AvatarClickFilter(i, m_userId, &m_dbOperator, this, dialog));
        
        int row = (i - 1) / 5;
        int col = (i - 1) % 5;
        gridLayout->addWidget(avatarLabel, row, col, Qt::AlignCenter);
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    // 添加取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *cancelBtn = new QPushButton("取消", dialog);
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #156080;"
        "    color: white;"
        "    border-radius: 5px;"
        "    padding: 8px 30px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1a7a9f;"
        "}"
    );
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);


    dialog->exec();
    delete dialog;
}

void UserMainWindow::loadUserInfo() {
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
        // 显示用户信息到个人中心页面
        ui->UsernameEdit->setText(userInfo.username);
        ui->PasswordEdit->setText(userInfo.password);
        ui->PhoneEdit->setText(userInfo.phone);
        ui->EmailEdit->setText(userInfo.email);
        ui->RealnameEdit->setText(userInfo.realname);
        ui->IdcardEdit->setText(userInfo.idcard);
    } else {
        QMessageBox::warning(this, "错误", "加载用户信息失败！");
    }
}

void UserMainWindow::clearFlightItems() {
    while (QLayoutItem* item = flightLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater(); // 安全删除Widget
        }
        delete item; // 删除布局项
    }
}

void UserMainWindow::on_searchBtn_clicked()
{
    // 每次查询前清空旧的条目，避免布局被填满或拉伸项挡住新内容
    clearFlightItems();

    QString departure = ui->departureCbox->currentText(); // 出发地下拉框
    QString destination = ui->arrivalCbox->currentText(); // 目的地下拉框
    QString year = ui->yearSpin->text();
    QString month = ui->monthSpin->text().rightJustified(2, '0'); // 补0（如“1”→“01”）
    QString day = ui->daySpin->text().rightJustified(2, '0');
    QString takeoffDate = QString("%1-%2-%3").arg(year, month, day);

    bool sf = false;
    // 使用DATE()函数匹配日期部分，因为departure_time是datetime类型
    QString sqlstr = QString("select * from flight_info where departure_city='%1' and arrival_city='%2' and DATE(departure_time)='%3'").arg(departure,destination,takeoffDate);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf)
    {
        QMessageBox::critical(this, "查询失败", "数据库错误：" + query.lastError().text());
        return;
    }

    //添加查询结果到滚动区域
    // 首先确保scrollAreaWidgetContents有正确的宽度
    int availableWidth = ui->scrollArea->viewport()->width() - 20; // 减去margins
    if (availableWidth > 0) {
        ui->scrollAreaWidgetContents->setMinimumWidth(availableWidth);
        ui->scrollAreaWidgetContents->resize(availableWidth, ui->scrollAreaWidgetContents->height());
    }
    
    int count = 0;
    while (query.next()) {
        count++;
        // 从查询结果中提取字段
        QString flightNo = query.value("flight_id").toString();
        
        // 解析并格式化时间：从数据库datetime转换为"2026年1月1日10:30"
        QDateTime takeoffDateTime = query.value("departure_time").toDateTime();
        QString takeoffTime = QString("%1年%2月%3日%4:%5")
            .arg(takeoffDateTime.date().year())
            .arg(takeoffDateTime.date().month())
            .arg(takeoffDateTime.date().day())
            .arg(takeoffDateTime.time().hour(), 2, 10, QChar('0'))
            .arg(takeoffDateTime.time().minute(), 2, 10, QChar('0'));
        
        // 解析并格式化到达时间
        QDateTime arriveDateTime = query.value("arrival_time").toDateTime();
        QString arriveTime = QString("%1年%2月%3日%4:%5")
            .arg(arriveDateTime.date().year())
            .arg(arriveDateTime.date().month())
            .arg(arriveDateTime.date().day())
            .arg(arriveDateTime.time().hour(), 2, 10, QChar('0'))
            .arg(arriveDateTime.time().minute(), 2, 10, QChar('0'));
        
        QString dep = query.value("departure_city").toString();
        QString dest = query.value("arrival_city").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(),query.value("total_seats").toString());

        // 创建航班条目Widget，父窗口必须是scrollAreaWidgetContents
        FlightItemWidget* itemWidget = new FlightItemWidget(
            flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, ui->scrollAreaWidgetContents
            );

        // 设置widget的大小策略，确保能够正确显示
        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        
        // 设置widget的宽度为父窗口可用宽度，避免超出
        int widgetWidth = availableWidth > 0 ? availableWidth : ui->scrollAreaWidgetContents->width() - 20;
        itemWidget->setFixedWidth(widgetWidth);
        itemWidget->setFixedHeight(229); // 固定高度
        
        // 确保widget可见
        itemWidget->show();

        // 连接条目内按钮的信号（处理预订/收藏）
        connect(itemWidget, &FlightItemWidget::bookClicked, this, &UserMainWindow::on_book_clicked);
        connect(itemWidget, &FlightItemWidget::collectClicked, this, &UserMainWindow::on_collect_clicked);

        // 添加到滚动区域的布局中
        flightLayout->addWidget(itemWidget);
    }
    
    if (count == 0) {
        QMessageBox::information(this, "查询结果", "未找到符合条件的航班");
        return;
    }
    
    // 追加弹性空间，保证列表顶部对齐并可滚动
    flightLayout->addStretch(1);
    
    // 强制布局重新计算大小
    flightLayout->invalidate(); // 使布局无效，强制重新计算
    flightLayout->update(); // 更新布局
    flightLayout->activate(); // 激活布局
    
    // 强制scrollAreaWidgetContents重新计算几何信息
    ui->scrollAreaWidgetContents->updateGeometry();
    
    // 计算内容所需的最小高度（包括margins和spacing）
    QSize layoutSize = flightLayout->sizeHint();
    int contentHeight = layoutSize.height();
    
    // 计算实际内容高度：每个widget高度 + 间距 + margins
    int actualHeight = count * 229 + (count - 1) * 3 + 10; // widget高度 + spacing + margins
    
    // 当widgetResizable为true时，scrollArea会自动调整widget大小
    // 但我们需要确保内容区域至少是内容的高度
    int minHeight = qMax(contentHeight, actualHeight);
    if (minHeight > 0) {
        ui->scrollAreaWidgetContents->setMinimumHeight(minHeight);
        ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->width(), minHeight);
    }
    
    // 更新scrollAreaWidgetContents的尺寸
    ui->scrollAreaWidgetContents->adjustSize();
    ui->scrollAreaWidgetContents->update(); // 强制更新显示
    
    // 确保scrollArea刷新显示
    ui->scrollArea->update();
    ui->scrollArea->viewport()->update();
}

void UserMainWindow::on_book_clicked(const QString& flightNo) {
    // 示例：弹出预订窗口，或向数据库写入订单
    QMessageBox::information(this, "预订成功", "已预订航班：" + flightNo);
}

void UserMainWindow::on_collect_clicked(const QString& flightNo) {
    // 示例：将航班号写入“我的收藏”表
    QMessageBox::information(this, "收藏成功", "已收藏航班：" + flightNo);
}
