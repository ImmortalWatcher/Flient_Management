#include "UserMainWindow.h"
#include "ui_UserMainWindow.h"

#include "FlightItemWidget.h"

#include <QDateTime>
#include <QDialog>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSqlError>
#include <QVBoxLayout>

// 构造函数：初始化用户主窗口
UserMainWindow::UserMainWindow(int userId, QWidget *parent) : QMainWindow(parent), ui(new Ui::UserMainWindow), m_userId(userId) {
    ui->setupUi(this);

    m_dbOperator.DBOpen();

    // 设置头像标签为可点击
    ui->avatarLabel->setCursor(Qt::PointingHandCursor);
    ui->avatarLabel->installEventFilter(this);

    loadUserInfo();
    loadAvatar();

    // 初始化航班列表布局
    flightLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    ui->scrollAreaWidgetContents->setLayout(flightLayout);
    flightLayout->setContentsMargins(10, 5, 10, 5);
    flightLayout->setAlignment(Qt::AlignTop);

    ui->scrollAreaWidgetContents->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->scrollArea->setWidgetResizable(false);
    ui->scrollAreaWidgetContents->show();

    connect(ui->searchBtn, &QPushButton::clicked, this, &UserMainWindow::on_searchBtn_clicked);
    connect(ui->resetBtn, &QPushButton::clicked, this, &UserMainWindow::on_resetBtn_clicked);

    loadAllFlights();
}

UserMainWindow::~UserMainWindow() {
    delete ui;
}

// 切换到航班查询页面
void UserMainWindow::on_flightQueryBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    loadAllFlights();
}

// 切换到我的订单页面
void UserMainWindow::on_myOrdersBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// 切换到我的收藏页面
void UserMainWindow::on_myFavoritesBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

// 切换到个人中心页面
void UserMainWindow::on_personalCenterBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    loadUserInfo();
}

// 退出登录
void UserMainWindow::on_backBtn_clicked() {
    emit logoutRequested();
    this->close();
}

// 事件过滤器：处理头像标签的点击事件
bool UserMainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->avatarLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            showAvatarselectionDialog();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

// 从数据库加载用户头像
void UserMainWindow::loadAvatar() {
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
        setAvatar(userInfo.avatarId);
    } else {
        setAvatar(1);  // 使用默认头像
    }
}

// 设置头像显示
void UserMainWindow::setAvatar(int avatarId) {
    QString avatarPath = QString(":/img/source/avatars/avatar_%1.png").arg(avatarId);
    ui->avatarLabel->setPixmap(QPixmap(avatarPath));
    ui->avatarLabel->setScaledContents(true);
}

// 显示头像选择对话框
void UserMainWindow::showAvatarselectionDialog() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择头像");
    dialog->setModal(true);
    dialog->resize(550, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    // 添加标题标签
    QLabel *titleLabel = new QLabel("请选择您的头像：", dialog);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    class AvatarClickFilter : public QObject {
    public:
        AvatarClickFilter(int avatarId, int userId, DBOperator *dbOperator, UserMainWindow *parent, QDialog *dialog) : QObject(parent), m_avatarId(avatarId), m_userId(userId), m_dbOperator(dbOperator), m_parent(parent), m_dialog(dialog) {}

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
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

    // 创建网格布局显示所有可选头像
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(20, 10, 20, 10);

    // 加载15个头像选项
    for (int i = 1; i <= 15; i++) {
        QString avatarPath = QString(":/img/source/avatars/avatar_%1.png").arg(i);

        QLabel *avatarLabel = new QLabel(dialog);
        avatarLabel->setPixmap(QPixmap(avatarPath));
        avatarLabel->setScaledContents(true);
        avatarLabel->setAlignment(Qt::AlignCenter);
        avatarLabel->setStyleSheet("QLabel {"
                                   "    border: 2px solid transparent;"
                                   "    padding: 5px;"
                                   "}"
                                   "QLabel:hover {"
                                   "    border: 2px solid #156080;"
                                   "    background-color: rgba(21, 96, 128, 0.1);"
                                   "}");
        avatarLabel->setCursor(Qt::PointingHandCursor);
        avatarLabel->setFixedSize(90, 90);

        // 为每个头像安装事件过滤器以处理点击
        avatarLabel->installEventFilter(new AvatarClickFilter(i, m_userId, &m_dbOperator, this, dialog));

        int row = (i - 1) / 5;
        int col = (i - 1) % 5;
        gridLayout->addWidget(avatarLabel, row, col, Qt::AlignCenter);
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *cancelBtn = new QPushButton("取消", dialog);
    cancelBtn->setStyleSheet("QPushButton {"
                             "    background-color: #156080;"
                             "    color: white;"
                             "    border-radius: 5px;"
                             "    padding: 8px 30px;"
                             "    font-size: 14px;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #1a7a9f;"
                             "}");
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);

    dialog->exec();
    delete dialog;
}

// 加载用户信息到个人中心页面
void UserMainWindow::loadUserInfo() {
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
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

// 清空航班列表中的所有条目
void UserMainWindow::clearFlightItems() {
    while (QLayoutItem *item = flightLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

// 加载所有航班信息
void UserMainWindow::loadAllFlights() {
    clearFlightItems();

    bool sf = false;
    QString sqlstr = "select * from flight_info ORDER BY departure_time";
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf) {
        QMessageBox::critical(this, "加载失败", "数据库错误：" + query.lastError().text());
        return;
    }

    // 设置滚动区域内容宽度
    int viewportWidth = ui->scrollArea->viewport()->width();
    int contentWidth = qMax(600, viewportWidth - 20);
    ui->scrollAreaWidgetContents->setMinimumWidth(contentWidth);
    ui->scrollAreaWidgetContents->resize(contentWidth, ui->scrollAreaWidgetContents->height());

    // 遍历查询结果，创建航班条目
    int count = 0;
    while (query.next()) {
        count++;
        QString flightNo = query.value("flight_id").toString();

        // 格式化起飞时间
        QDateTime takeoffDateTime = query.value("departure_time").toDateTime();
        QString takeoffTime = QString("%1年%2月%3日%4:%5").arg(takeoffDateTime.date().year()).arg(takeoffDateTime.date().month()).arg(takeoffDateTime.date().day()).arg(takeoffDateTime.time().hour(), 2, 10, QChar('0')).arg(takeoffDateTime.time().minute(), 2, 10, QChar('0'));

        // 格式化到达时间
        QDateTime arriveDateTime = query.value("arrival_time").toDateTime();
        QString arriveTime = QString("%1年%2月%3日%4:%5").arg(arriveDateTime.date().year()).arg(arriveDateTime.date().month()).arg(arriveDateTime.date().day()).arg(arriveDateTime.time().hour(), 2, 10, QChar('0')).arg(arriveDateTime.time().minute(), 2, 10, QChar('0'));

        QString dep = query.value("departure_city").toString();
        QString dest = query.value("arrival_city").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
        QString airlineCompany = query.value("airline_company").toString();

        // 创建航班条目组件并添加到布局
        FlightItemWidget *itemWidget = new FlightItemWidget(flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, airlineCompany, ui->scrollAreaWidgetContents);

        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemWidget->setFixedWidth(600);
        itemWidget->setFixedHeight(229);
        itemWidget->show();

        // 连接预订和收藏按钮信号
        connect(itemWidget, &FlightItemWidget::bookClicked, this, &UserMainWindow::on_book_clicked);
        connect(itemWidget, &FlightItemWidget::collectClicked, this, &UserMainWindow::on_collect_clicked);

        flightLayout->addWidget(itemWidget);
    }

    if (count == 0) {
        return;
    }

    flightLayout->addStretch(1);
    flightLayout->invalidate();
    flightLayout->update();
    flightLayout->activate();

    ui->scrollAreaWidgetContents->updateGeometry();

    ui->scrollAreaWidgetContents->adjustSize();
    ui->scrollAreaWidgetContents->update();
    ui->scrollArea->update();
    ui->scrollArea->viewport()->update();
}

// 重置查询条件并显示所有航班
void UserMainWindow::on_resetBtn_clicked() {
    // 重置出发地和目的地下拉框到第一个选项
    ui->departureCbox->setCurrentIndex(0);
    ui->arrivalCbox->setCurrentIndex(0);

    // 重置日期到初始值 (2025 年 1 月 1 日)
    ui->yearSpin->setValue(2025);
    ui->monthSpin->setValue(1);
    ui->daySpin->setValue(1);

    // 加载所有航班
    loadAllFlights();
}

// 根据条件搜索航班
void UserMainWindow::on_searchBtn_clicked() {
    clearFlightItems();

    // 获取搜索条件
    QString departure = ui->departureCbox->currentText();
    QString destination = ui->arrivalCbox->currentText();
    QString year = ui->yearSpin->text();
    QString month = ui->monthSpin->text().rightJustified(2, '0');
    QString day = ui->daySpin->text().rightJustified(2, '0');
    QString takeoffDate = QString("%1-%2-%3").arg(year, month, day);

    // 执行搜索查询
    bool sf = false;
    QString sqlstr = QString("select * from flight_info where departure_city='%1' AND arrival_city='%2' AND DATE(departure_time)='%3' ORDER BY departure_time").arg(departure, destination, takeoffDate);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf) {
        QMessageBox::critical(this, "查询失败", "数据库错误：" + query.lastError().text());
        return;
    }

    int viewportWidth = ui->scrollArea->viewport()->width();
    int contentWidth = qMax(600, viewportWidth - 20);
    ui->scrollAreaWidgetContents->setMinimumWidth(contentWidth);
    ui->scrollAreaWidgetContents->resize(contentWidth, ui->scrollAreaWidgetContents->height());

    int count = 0;
    while (query.next()) {
        count++;
        QString flightNo = query.value("flight_id").toString();

        QDateTime takeoffDateTime = query.value("departure_time").toDateTime();
        QString takeoffTime = QString("%1年%2月%3日%4:%5").arg(takeoffDateTime.date().year()).arg(takeoffDateTime.date().month()).arg(takeoffDateTime.date().day()).arg(takeoffDateTime.time().hour(), 2, 10, QChar('0')).arg(takeoffDateTime.time().minute(), 2, 10, QChar('0'));

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
        QString remaining = QString("%1/%2")
                                .arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
        QString airlineCompany = query.value("airline_company").toString();

        FlightItemWidget *itemWidget = new FlightItemWidget(flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, airlineCompany, ui->scrollAreaWidgetContents);

        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemWidget->setFixedWidth(600);
        itemWidget->setFixedHeight(229);
        itemWidget->show();

        connect(itemWidget, &FlightItemWidget::bookClicked, this, &UserMainWindow::on_book_clicked);
        connect(itemWidget, &FlightItemWidget::collectClicked, this, &UserMainWindow::on_collect_clicked);

        flightLayout->addWidget(itemWidget);
    }

    if (count == 0) {
        QMessageBox::information(this, "查询结果", "未找到符合条件的航班");
        return;
    }

    flightLayout->addStretch(1);
    flightLayout->invalidate();
    flightLayout->update();
    flightLayout->activate();

    ui->scrollAreaWidgetContents->updateGeometry();

    ui->scrollAreaWidgetContents->adjustSize();
    ui->scrollAreaWidgetContents->update();
    ui->scrollArea->update();
    ui->scrollArea->viewport()->update();
}

// 处理航班预订按钮点击
void UserMainWindow::on_book_clicked(const QString &flightNo) {
    QMessageBox::information(this, "预订成功", "已预订航班：" + flightNo);
}

// 处理航班收藏按钮点击
void UserMainWindow::on_collect_clicked(const QString &flightNo) {
    QMessageBox::information(this, "收藏成功", "已收藏航班：" + flightNo);
}
