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
    flightLayout->setSpacing(10); // 条目之间的间距
    flightLayout->setContentsMargins(10, 10, 10, 10);
    flightLayout->setAlignment(Qt::AlignTop); // 确保条目从顶部开始排列
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
    QString sqlstr = QString("select * from flight_info where departure_city='%1' and arrival_city='%2' and departure_time='%3'").arg(departure,destination,takeoffDate);
    // qDebug() << "执行的 SQL 语句:" << sqlstr;
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf)
    {
        QMessageBox::critical(this, "查询失败", "数据库错误：" + query.lastError().text());
        return;
    }

    //添加查询结果到滚动区域

    while (query.next()) {
        // 从查询结果中提取字段
        QString flightNo = query.value("flight_id").toString();
        QString takeoffTime = query.value("departure_time").toString(); // 格式如“2025-01-01 10:30”
        QString dep = query.value("departure_city").toString();
        QString dest = query.value("arrival_city").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/80").arg(query.value("remaining_seats").toString()); // 假设总座位80

        // 创建航班条目Widget
        FlightItemWidget* itemWidget = new FlightItemWidget(
            flightNo, takeoffTime, dep, dest, price, remaining, this
            );

        // 连接条目内按钮的信号（处理预订/收藏）
        connect(itemWidget, &FlightItemWidget::bookClicked, this, &UserMainWindow::on_book_clicked);
        connect(itemWidget, &FlightItemWidget::collectClicked, this, &UserMainWindow::on_collect_clicked);

        // 添加到滚动区域的布局中
        flightLayout->addWidget(itemWidget);
        qDebug()<<"成功创建widget";
    }
    // 追加弹性空间，保证列表顶部对齐并可滚动
    flightLayout->addStretch(1);
    ui->scrollAreaWidgetContents->adjustSize(); // 刷新内容区域尺寸以触发重绘
}

void UserMainWindow::on_book_clicked(const QString& flightNo) {
    // 示例：弹出预订窗口，或向数据库写入订单
    QMessageBox::information(this, "预订成功", "已预订航班：" + flightNo);
}

void UserMainWindow::on_collect_clicked(const QString& flightNo) {
    // 示例：将航班号写入“我的收藏”表
    QMessageBox::information(this, "收藏成功", "已收藏航班：" + flightNo);
}
