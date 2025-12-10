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

    // 初始化编辑模式相关变量
    m_isEditMode = false;
    m_usernameEdit = nullptr;
    m_passwordEdit = nullptr;
    m_phoneEdit = nullptr;
    m_emailEdit = nullptr;
    m_realnameEdit = nullptr;
    m_idcardEdit = nullptr;
    m_saveBtn = nullptr;
    m_cancelBtn = nullptr;

    loadUserInfo();
    loadAvatar();

    // 初始化航班列表布局
    flightLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    ui->scrollAreaWidgetContents->setLayout(flightLayout);
    flightLayout->setContentsMargins(10, 5, 10, 5);
    flightLayout->setAlignment(Qt::AlignTop);

    ui->scrollAreaWidgetContents->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->scrollArea_1->setWidgetResizable(false);
    ui->scrollAreaWidgetContents->show();

    // 初始化订单列表布局
    orderLayout = new QVBoxLayout(ui->scrollAreaWidgetContents_2);
    ui->scrollAreaWidgetContents_2->setLayout(orderLayout);
    orderLayout->setContentsMargins(10, 5, 10, 5);
    orderLayout->setAlignment(Qt::AlignTop);

    loadAllFlights();
}

UserMainWindow::~UserMainWindow() {
    // 如果处于编辑模式，先退出编辑模式以清理资源
    if (m_isEditMode) {
        exitEditMode();
    }
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
    loadOrders(); // 加载订单列表
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
        setAvatar(1); // 使用默认头像
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
        ui->UsernameLabel->setText(userInfo.username);
        ui->PasswordLabel->setText(userInfo.password);
        ui->PhoneLabel->setText(userInfo.phone);
        ui->EmailLabel->setText(userInfo.email);
        ui->RealnameLabel->setText(userInfo.realname);
        ui->IdcardLabel->setText(userInfo.idcard);
        ui->BalanceLabel->setText(QString::number(userInfo.balance, 'f', 2) + " 元");
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
    int viewportWidth = ui->scrollArea_1->viewport()->width();
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

        QString dep = query.value("departure_airport").toString();
        QString dest = query.value("arrival_airport").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
        QString airlineCompany = query.value("airline_company").toString();

        // 创建航班条目组件并添加到布局
        FlightItemWidget *itemWidget = new FlightItemWidget(flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, airlineCompany, ui->scrollAreaWidgetContents);

        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemWidget->setFixedWidth(600);
        itemWidget->setFixedHeight(180);
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
    ui->scrollArea_1->update();
    ui->scrollArea_1->viewport()->update();
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

    int viewportWidth = ui->scrollArea_1->viewport()->width();
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
        QString arriveTime = QString("%1年%2月%3日%4:%5").arg(arriveDateTime.date().year()).arg(arriveDateTime.date().month()).arg(arriveDateTime.date().day()).arg(arriveDateTime.time().hour(), 2, 10, QChar('0')).arg(arriveDateTime.time().minute(), 2, 10, QChar('0'));

        QString dep = query.value("departure_airport").toString();
        QString dest = query.value("arrival_airport").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
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
    ui->scrollArea_1->update();
    ui->scrollArea_1->viewport()->update();
}

// 处理航班预订按钮点击
void UserMainWindow::on_book_clicked(const QString &flightNo) {
    // 查询航班详细信息
    bool success = false;
    QString sqlstr = QString("SELECT * FROM flight_info WHERE flight_id='%1'").arg(flightNo);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);
    
    if (!success || !query.next()) {
        QMessageBox::warning(this, "错误", "获取航班信息失败！");
        return;
    }
    
    // 获取航班信息
    QString departureAirport = query.value("departure_airport").toString();
    QString arrivalAirport = query.value("arrival_airport").toString();
    QDateTime departureTime = query.value("departure_time").toDateTime();
    QDateTime arrivalTime = query.value("arrival_time").toDateTime();
    double price = query.value("price").toDouble();
    
    // 格式化时间
    QString takeoffTime = QString("%1年%2月%3日 %4:%5")
                          .arg(departureTime.date().year())
                          .arg(departureTime.date().month())
                          .arg(departureTime.date().day())
                          .arg(departureTime.time().hour(), 2, 10, QChar('0'))
                          .arg(departureTime.time().minute(), 2, 10, QChar('0'));
    
    QString arriveTime = QString("%1年%2月%3日 %4:%5")
                         .arg(arrivalTime.date().year())
                         .arg(arrivalTime.date().month())
                         .arg(arrivalTime.date().day())
                         .arg(arrivalTime.time().hour(), 2, 10, QChar('0'))
                         .arg(arrivalTime.time().minute(), 2, 10, QChar('0'));
    
    // 创建预订对话框
    QDialog *bookDialog = new QDialog(this);
    bookDialog->setWindowTitle("预订航班");
    bookDialog->setModal(true);
    bookDialog->resize(450, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(bookDialog);
    
    // 添加标题
    QLabel *titleLabel = new QLabel("航班预订信息", bookDialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // 显示航班信息
    QLabel *flightNoLabel = new QLabel(QString("航班号：%1").arg(flightNo), bookDialog);
    flightNoLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(flightNoLabel);
    
    QLabel *departureLabel = new QLabel(QString("出发地：%1").arg(departureAirport), bookDialog);
    departureLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(departureLabel);
    
    QLabel *arrivalLabel = new QLabel(QString("目的地：%1").arg(arrivalAirport), bookDialog);
    arrivalLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(arrivalLabel);
    
    QLabel *takeoffLabel = new QLabel(QString("起飞时间：%1").arg(takeoffTime), bookDialog);
    takeoffLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(takeoffLabel);
    
    QLabel *arriveLabel = new QLabel(QString("到达时间：%1").arg(arriveTime), bookDialog);
    arriveLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(arriveLabel);
    
    QLabel *priceLabel = new QLabel(QString("票价：%1 元").arg(QString::number(price, 'f', 2)), bookDialog);
    priceLabel->setStyleSheet("font-size: 14px; padding: 5px; color: #d9534f; font-weight: bold;");
    mainLayout->addWidget(priceLabel);
    
    // 分隔线
    QFrame *line = new QFrame(bookDialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    
    // 获取用户信息
    DBOperator::UserInfo userInfo;
    if (!m_dbOperator.getUserInfo(m_userId, userInfo)) {
        QMessageBox::warning(this, "错误", "获取用户信息失败！");
        delete bookDialog;
        return;
    }
    
    // 显示乘客信息（使用用户信息）
    QLabel *passengerTitle = new QLabel("乘客信息", bookDialog);
    passengerTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px 5px;");
    mainLayout->addWidget(passengerTitle);
    
    QLabel *nameLabel = new QLabel(QString("乘客姓名：%1").arg(userInfo.realname), bookDialog);
    nameLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(nameLabel);
    
    QLabel *idcardLabel = new QLabel(QString("身份证号：%1").arg(userInfo.idcard), bookDialog);
    idcardLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    mainLayout->addWidget(idcardLabel);
    
    mainLayout->addStretch();
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton *confirmBtn = new QPushButton("确认订票", bookDialog);
    confirmBtn->setStyleSheet("QPushButton {"
                             "    background-color: #156080;"
                             "    color: white;"
                             "    border-radius: 5px;"
                             "    padding: 8px 30px;"
                             "    font-size: 14px;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #1a7a9f;"
                             "}");
    buttonLayout->addWidget(confirmBtn);
    
    QPushButton *cancelBtn = new QPushButton("取消", bookDialog);
    cancelBtn->setStyleSheet("QPushButton {"
                            "    background-color: #999;"
                            "    color: white;"
                            "    border-radius: 5px;"
                            "    padding: 8px 30px;"
                            "    font-size: 14px;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #777;"
                            "}");
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);
    
    // 连接确认按钮
    connect(confirmBtn, &QPushButton::clicked, bookDialog, [=]() {
        // 直接使用用户信息
        QString passengerName = userInfo.realname;
        QString passengerIdcard = userInfo.idcard;
        
        if (userInfo.balance < price) {
            QMessageBox::warning(bookDialog, "余额不足", QString("账户余额不足！\n当前余额：%1 元\n需要支付：%2 元").arg(QString::number(userInfo.balance, 'f', 2), QString::number(price, 'f', 2)));
            return;
        }
        
        // 检查剩余座位
        if (query.value("remaining_seats").toInt() <= 0) {
            QMessageBox::warning(bookDialog, "预订失败", "该航班已无剩余座位！");
            return;
        }
        
        // 扣除余额
        double newBalance = userInfo.balance - price;
        bool updateSuccess = false;
        QString updateSql = QString("UPDATE user_info SET balance=%1 WHERE id=%2").arg(newBalance).arg(m_userId);
        QSqlQuery updateQuery = m_dbOperator.DBGetData(updateSql, updateSuccess);
        
        if (!updateSuccess) {
            QMessageBox::warning(bookDialog, "预订失败", "更新余额失败：" + updateQuery.lastError().text());
            return;
        }
        
        // 创建订单
        QString insertSql = QString("INSERT INTO order_info (user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price) "
                                   "VALUES (%1, '%2', '%3', '%4', '%5', '%6', '%7', '%8', %9)")
                           .arg(m_userId)
                           .arg(flightNo)
                           .arg(passengerName)
                           .arg(passengerIdcard)
                           .arg(departureAirport)
                           .arg(arrivalAirport)
                           .arg(departureTime.toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(arrivalTime.toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(price);
        
        bool insertSuccess = false;
        QSqlQuery insertQuery = m_dbOperator.DBGetData(insertSql, insertSuccess);
        
        if (!insertSuccess) {
            // 如果订单创建失败，回滚余额
            QString rollbackSql = QString("UPDATE user_info SET balance=%1 WHERE id=%2").arg(userInfo.balance).arg(m_userId);
            m_dbOperator.DBGetData(rollbackSql, updateSuccess);
            QMessageBox::warning(bookDialog, "预订失败", "创建订单失败：" + insertQuery.lastError().text());
            return;
        }
        
        // 更新航班剩余座位
        int remainingSeats = query.value("remaining_seats").toInt() - 1;
        QString seatSql = QString("UPDATE flight_info SET remaining_seats=%1 WHERE flight_id='%2'").arg(remainingSeats).arg(flightNo);
        m_dbOperator.DBGetData(seatSql, updateSuccess);
        
        // 刷新用户信息和订单列表
        loadUserInfo();
        loadOrders();
        
        QMessageBox::information(bookDialog, "预订成功", QString("预订成功！\n订单已创建，余额已扣除 %1 元\n当前余额：%2 元").arg(QString::number(price, 'f', 2), QString::number(newBalance, 'f', 2)));
        bookDialog->accept();
    });
    
    // 连接取消按钮
    connect(cancelBtn, &QPushButton::clicked, bookDialog, &QDialog::reject);
    
    bookDialog->exec();
    delete bookDialog;
}

// 处理航班收藏按钮点击
void UserMainWindow::on_collect_clicked(const QString &flightNo) {
    QMessageBox::information(this, "收藏成功", "已收藏航班：" + flightNo);
}

// 处理编辑个人信息按钮点击，进入编辑模式
void UserMainWindow::on_editInfoBtn_clicked() {
    enterEditMode();
}

// 处理保存按钮点击，保存用户信息
void UserMainWindow::on_saveBtn_clicked() {
    saveUserInfo();
    exitEditMode();
}

// 处理取消按钮点击，退出编辑模式并恢复原始数据
void UserMainWindow::on_cancelBtn_clicked() {
    ui->UsernameLabel->setText(m_originalUserInfo.username);
    ui->PasswordLabel->setText(m_originalUserInfo.password);
    ui->PhoneLabel->setText(m_originalUserInfo.phone);
    ui->EmailLabel->setText(m_originalUserInfo.email);
    ui->RealnameLabel->setText(m_originalUserInfo.realname);
    ui->IdcardLabel->setText(m_originalUserInfo.idcard);

    exitEditMode();
}

// 处理注销账号按钮点击
void UserMainWindow::on_cancelAccountBtn_clicked() {
    // 显示确认对话框，提示用户一旦注销账号不可找回
    int ret = QMessageBox::question(this, "确认注销", "一旦注销，账号不可找回。\n确定要注销账号吗？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes) {
        return;
    }

    // 从数据库中删除用户信息
    bool success = false;
    QString sqlstr = QString("DELETE FROM user_info WHERE id=%1").arg(m_userId);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);

    if (!success) {
        QMessageBox::warning(this, "注销失败", "数据库错误：" + query.lastError().text());
        return;
    }

    // 注销成功，显示提示信息并退出登录
    QMessageBox::information(this, "注销成功", "账号已成功注销");
    emit logoutRequested();
    this->close();
}

// 处理充值按钮点击
void UserMainWindow::on_rechargeBtn_clicked() {
    // 创建充值对话框
    QDialog *rechargeDialog = new QDialog(this);
    rechargeDialog->setWindowTitle("账户充值");
    rechargeDialog->setModal(true);
    rechargeDialog->resize(350, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(rechargeDialog);

    // 添加标题标签
    QLabel *titleLabel = new QLabel("请输入充值金额：", rechargeDialog);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // 创建输入框
    QLineEdit *amountEdit = new QLineEdit(rechargeDialog);
    amountEdit->setPlaceholderText("请输入充值金额（元）");
    amountEdit->setStyleSheet("font-size: 14px; padding: 8px;");
    mainLayout->addWidget(amountEdit);

    // 显示当前余额
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
        QLabel *balanceLabel = new QLabel(QString("当前余额：%1 元").arg(QString::number(userInfo.balance, 'f', 2)), rechargeDialog);
        balanceLabel->setStyleSheet("font-size: 12px; color: #666; padding: 5px;");
        mainLayout->addWidget(balanceLabel);
    }

    mainLayout->addStretch();

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *confirmBtn = new QPushButton("确认充值", rechargeDialog);
    confirmBtn->setStyleSheet("QPushButton {"
                             "    background-color: #156080;"
                             "    color: white;"
                             "    border-radius: 5px;"
                             "    padding: 8px 30px;"
                             "    font-size: 14px;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #1a7a9f;"
                             "}");
    buttonLayout->addWidget(confirmBtn);

    QPushButton *cancelBtn = new QPushButton("取消", rechargeDialog);
    cancelBtn->setStyleSheet("QPushButton {"
                            "    background-color: #999;"
                            "    color: white;"
                            "    border-radius: 5px;"
                            "    padding: 8px 30px;"
                            "    font-size: 14px;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #777;"
                            "}");
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);

    // 连接确认按钮
    connect(confirmBtn, &QPushButton::clicked, rechargeDialog, [=]() {
        QString amountStr = amountEdit->text().trimmed();

        // 验证输入是否为空
        if (amountStr.isEmpty()) {
            QMessageBox::warning(rechargeDialog, "警告", "请输入充值金额！");
            return;
        }

        // 验证输入是否为有效数字
        bool ok;
        double amount = amountStr.toDouble(&ok);
        if (!ok || amount <= 0) {
            QMessageBox::warning(rechargeDialog, "警告", "请输入有效的充值金额（必须大于0）！");
            return;
        }

        // 获取当前余额
        DBOperator::UserInfo currentUserInfo;
        if (!m_dbOperator.getUserInfo(m_userId, currentUserInfo)) {
            QMessageBox::warning(rechargeDialog, "错误", "获取用户信息失败！");
            return;
        }

        // 计算新余额
        double newBalance = currentUserInfo.balance + amount;

        // 更新数据库中的余额
        bool success = false;
        QString sqlstr = QString("UPDATE user_info SET balance=%1 WHERE id=%2").arg(newBalance).arg(m_userId);
        QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);
        
        if (!success) {
            QMessageBox::warning(rechargeDialog, "充值失败", "数据库错误：" + query.lastError().text());
            return;
        }

        // 充值成功，刷新余额显示
        loadUserInfo();
        QMessageBox::information(rechargeDialog, "充值成功", QString("充值成功！\n充值金额：%1 元\n当前余额：%2 元").arg(QString::number(amount, 'f', 2), QString::number(newBalance, 'f', 2)));
        rechargeDialog->accept();
    });

    // 连接取消按钮
    connect(cancelBtn, &QPushButton::clicked, rechargeDialog, &QDialog::reject);

    rechargeDialog->exec();
    delete rechargeDialog;
}

// 清空订单列表
void UserMainWindow::clearOrders() {
    while (QLayoutItem *item = orderLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

// 加载订单列表
void UserMainWindow::loadOrders() {
    clearOrders();

    // 查询当前用户的所有订单
    bool success = false;
    QString sqlstr = QString("SELECT * FROM order_info WHERE user_id=%1 ORDER BY order_time DESC").arg(m_userId);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);

    if (!success) {
        QMessageBox::warning(this, "加载失败", "数据库错误：" + query.lastError().text());
        return;
    }

    int count = 0;
    while (query.next()) {
        count++;
        int orderId = query.value("order_id").toInt();
        QString flightId = query.value("flight_id").toString();
        QString passengerName = query.value("passenger_name").toString();
        QString passengerIdcard = query.value("passenger_idcard").toString();
        QString departureCity = query.value("departure_city").toString();
        QString arrivalCity = query.value("arrival_city").toString();
        QDateTime departureTime = query.value("departure_time").toDateTime();
        QDateTime arrivalTime = query.value("arrival_time").toDateTime();
        double price = query.value("price").toDouble();
        QDateTime orderTime = query.value("order_time").toDateTime();

        // 生成订单号
        QString orderNo = QString("%1%2%3%4%5%6%7").arg(orderTime.date().year(), 4, 10, QChar('0')).arg(orderTime.date().month(), 2, 10, QChar('0')).arg(orderTime.date().day(), 2, 10, QChar('0')).arg(orderTime.time().hour(), 2, 10, QChar('0')).arg(orderTime.time().minute(), 2, 10, QChar('0')).arg(orderTime.time().second(), 2, 10, QChar('0')).arg(orderId, 3, 10, QChar('0'));

        // 格式化时间
        QString takeoffTime = QString("%1年%2月%3日 %4:%5").arg(departureTime.date().year()).arg(departureTime.date().month()).arg(departureTime.date().day()).arg(departureTime.time().hour(), 2, 10, QChar('0')).arg(departureTime.time().minute(), 2, 10, QChar('0'));

        QString arriveTime = QString("%1年%2月%3日 %4:%5").arg(arrivalTime.date().year()).arg(arrivalTime.date().month()).arg(arrivalTime.date().day()).arg(arrivalTime.time().hour(), 2, 10, QChar('0')).arg(arrivalTime.time().minute(), 2, 10, QChar('0'));

        // 创建订单显示组件
        QFrame *orderFrame = new QFrame(ui->scrollAreaWidgetContents_2);
        orderFrame->setStyleSheet("QFrame {"
                                  "    background-color: #f5f5f5;"
                                  "    border: 1px solid #ddd;"
                                  "    border-radius: 8px;"
                                  "    padding: 10px;"
                                  "}");
        orderFrame->setFixedHeight(280);

        QVBoxLayout *orderFrameLayout = new QVBoxLayout(orderFrame);
        orderFrameLayout->setContentsMargins(15, 15, 15, 15);
        orderFrameLayout->setSpacing(8);

        // 订单号
        QLabel *orderIdLabel = new QLabel(QString("订单号：%1").arg(orderNo), orderFrame);
        orderIdLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #156080;");
        orderFrameLayout->addWidget(orderIdLabel);

        // 航班信息
        QLabel *flightInfoLabel = new QLabel(QString("航班号：%1 | %2 → %3").arg(flightId, departureCity, arrivalCity), orderFrame);
        flightInfoLabel->setStyleSheet("font-size: 13px;");
        orderFrameLayout->addWidget(flightInfoLabel);
        
        // 时间信息
        QLabel *timeLabel = new QLabel(QString("起飞：%1 | 到达：%2").arg(takeoffTime, arriveTime), orderFrame);
        timeLabel->setStyleSheet("font-size: 13px;");
        orderFrameLayout->addWidget(timeLabel);

        // 乘客信息
        QLabel *passengerLabel = new QLabel(QString("乘客：%1 | 身份证：%2").arg(passengerName, passengerIdcard), orderFrame);
        passengerLabel->setStyleSheet("font-size: 13px;");
        orderFrameLayout->addWidget(passengerLabel);

        // 价格和下单时间
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        QLabel *priceLabel = new QLabel(QString("票价：%1 元").arg(QString::number(price, 'f', 2)), orderFrame);
        priceLabel->setStyleSheet("font-size: 13px; color: #d9534f; font-weight: bold;");
        bottomLayout->addWidget(priceLabel);

        bottomLayout->addStretch();

        QString orderTimeStr = QString("%1年%2月%3日 %4:%5").arg(orderTime.date().year()).arg(orderTime.date().month()).arg(orderTime.date().day()).arg(orderTime.time().hour(), 2, 10, QChar('0')).arg(orderTime.time().minute(), 2, 10, QChar('0'));
        QLabel *orderTimeLabel = new QLabel(QString("下单时间：%1").arg(orderTimeStr), orderFrame);
        orderTimeLabel->setStyleSheet("font-size: 13px; color: #999;");
        bottomLayout->addWidget(orderTimeLabel);

        orderFrameLayout->addLayout(bottomLayout);

        orderLayout->addWidget(orderFrame);
    }
    
    if (count == 0) {
        QLabel *noOrderLabel = new QLabel("暂无订单", ui->scrollAreaWidgetContents_2);
        noOrderLabel->setStyleSheet("font-size: 16px; color: #999; padding: 50px;");
        noOrderLabel->setAlignment(Qt::AlignCenter);
        orderLayout->addWidget(noOrderLabel);
    }
    
    orderLayout->addStretch();
}

// 进入编辑模式：将标签转换为可编辑的输入框，隐藏原按钮，显示保存和取消按钮
void UserMainWindow::enterEditMode() {
    if (m_isEditMode) {
        return;
    }

    m_isEditMode = true;

    // 保存原始用户信息
    m_dbOperator.getUserInfo(m_userId, m_originalUserInfo);

    // 获取各个标签的位置和大小
    QRect usernameRect = ui->UsernameLabel->geometry();
    QRect passwordRect = ui->PasswordLabel->geometry();
    QRect phoneRect = ui->PhoneLabel->geometry();
    QRect emailRect = ui->EmailLabel->geometry();
    QRect realnameRect = ui->RealnameLabel->geometry();
    QRect idcardRect = ui->IdcardLabel->geometry();

    // 创建用户名输入框
    m_usernameEdit = new QLineEdit(ui->page_4);
    m_usernameEdit->setGeometry(usernameRect);
    m_usernameEdit->setText(ui->UsernameLabel->text());
    m_usernameEdit->setFont(ui->UsernameLabel->font());
    m_usernameEdit->show();
    ui->UsernameLabel->hide();

    // 创建密码输入框
    m_passwordEdit = new QLineEdit(ui->page_4);
    m_passwordEdit->setGeometry(passwordRect);
    m_passwordEdit->setText(ui->PasswordLabel->text());
    m_passwordEdit->setFont(ui->PasswordLabel->font());
    m_passwordEdit->setEchoMode(QLineEdit::Normal);
    m_passwordEdit->show();
    ui->PasswordLabel->hide();

    // 创建手机号输入框
    m_phoneEdit = new QLineEdit(ui->page_4);
    m_phoneEdit->setGeometry(phoneRect);
    m_phoneEdit->setText(ui->PhoneLabel->text());
    m_phoneEdit->setFont(ui->PhoneLabel->font());
    m_phoneEdit->show();
    ui->PhoneLabel->hide();
    
    // 创建邮箱输入框
    m_emailEdit = new QLineEdit(ui->page_4);
    m_emailEdit->setGeometry(emailRect);
    m_emailEdit->setText(ui->EmailLabel->text());
    m_emailEdit->setFont(ui->EmailLabel->font());
    m_emailEdit->show();
    ui->EmailLabel->hide();
    
    // 创建真实姓名输入框
    m_realnameEdit = new QLineEdit(ui->page_4);
    m_realnameEdit->setGeometry(realnameRect);
    m_realnameEdit->setText(ui->RealnameLabel->text());
    m_realnameEdit->setFont(ui->RealnameLabel->font());
    m_realnameEdit->show();
    ui->RealnameLabel->hide();
    
    // 创建身份证号输入框
    m_idcardEdit = new QLineEdit(ui->page_4);
    m_idcardEdit->setGeometry(idcardRect);
    m_idcardEdit->setText(ui->IdcardLabel->text());
    m_idcardEdit->setFont(ui->IdcardLabel->font());
    m_idcardEdit->show();
    ui->IdcardLabel->hide();

    // 隐藏原来的 3 个按钮
    ui->editInfoBtn->hide();
    ui->rechargeBtn->hide();
    ui->cancelAccountBtn->hide();

    // 创建并显示保存按钮 (向右平移)
    m_saveBtn = new QPushButton(ui->page_4);
    QRect saveBtnRect = ui->editInfoBtn->geometry();
    saveBtnRect.moveLeft(saveBtnRect.x() + 105);
    m_saveBtn->setGeometry(saveBtnRect);
    m_saveBtn->setText("保存");
    m_saveBtn->setFont(ui->editInfoBtn->font());
    m_saveBtn->setStyleSheet(ui->editInfoBtn->styleSheet());
    m_saveBtn->show();
    connect(m_saveBtn, &QPushButton::clicked, this, &UserMainWindow::on_saveBtn_clicked);

    // 创建并显示取消按钮 (向右平移)
    m_cancelBtn = new QPushButton(ui->page_4);
    QRect cancelBtnRect = ui->rechargeBtn->geometry();
    cancelBtnRect.moveLeft(cancelBtnRect.x() + 115);
    m_cancelBtn->setGeometry(cancelBtnRect);
    m_cancelBtn->setText("取消");
    m_cancelBtn->setFont(ui->rechargeBtn->font());
    m_cancelBtn->setStyleSheet(ui->rechargeBtn->styleSheet());
    m_cancelBtn->show();
    connect(m_cancelBtn, &QPushButton::clicked, this, &UserMainWindow::on_cancelBtn_clicked);
}

// 退出编辑模式：恢复标签显示，隐藏输入框，恢复原按钮
void UserMainWindow::exitEditMode() {
    if (!m_isEditMode) {
        return;
    }

    m_isEditMode = false;

    // 恢复标签显示
    if (m_usernameEdit) {
        ui->UsernameLabel->show();
        m_usernameEdit->hide();
        m_usernameEdit->deleteLater();
        m_usernameEdit = nullptr;
    }

    if (m_passwordEdit) {
        ui->PasswordLabel->show();
        m_passwordEdit->hide();
        m_passwordEdit->deleteLater();
        m_passwordEdit = nullptr;
    }

    if (m_phoneEdit) {
        ui->PhoneLabel->show();
        m_phoneEdit->hide();
        m_phoneEdit->deleteLater();
        m_phoneEdit = nullptr;
    }

    if (m_emailEdit) {
        ui->EmailLabel->show();
        m_emailEdit->hide();
        m_emailEdit->deleteLater();
        m_emailEdit = nullptr;
    }

    if (m_realnameEdit) {
        ui->RealnameLabel->show();
        m_realnameEdit->hide();
        m_realnameEdit->deleteLater();
        m_realnameEdit = nullptr;
    }

    if (m_idcardEdit) {
        ui->IdcardLabel->show();
        m_idcardEdit->hide();
        m_idcardEdit->deleteLater();
        m_idcardEdit = nullptr;
    }

    // 恢复原按钮显示
    ui->editInfoBtn->show();
    ui->rechargeBtn->show();
    ui->cancelAccountBtn->show();

    // 删除保存和取消按钮
    if (m_saveBtn) {
        m_saveBtn->hide();
        m_saveBtn->deleteLater();
        m_saveBtn = nullptr;
    }

    if (m_cancelBtn) {
        m_cancelBtn->hide();
        m_cancelBtn->deleteLater();
        m_cancelBtn = nullptr;
    }
}

// 保存用户信息到数据库
void UserMainWindow::saveUserInfo() {
    if (!m_isEditMode) {
        return;
    }

    // 获取输入框中的值
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text().trimmed();
    QString phone = m_phoneEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString realname = m_realnameEdit->text().trimmed();
    QString idcard = m_idcardEdit->text().trimmed();

    // 验证用户名是否为空
    if (username.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名不能为空");
        return;
    }

    // 验证密码是否为空
    if (password.isEmpty()) {
        QMessageBox::warning(this, "警告", "密码不能为空");
        return;
    }

    // 验证手机号格式
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "警告", "手机号不能为空");
        return;
    }
    if (phone.length() != 11 || !phone.toLongLong()) {
        QMessageBox::warning(this, "警告", "手机号格式不正确，请输入11位数字");
        return;
    }

    // 验证邮箱格式
    if (email.isEmpty()) {
        QMessageBox::warning(this, "警告", "邮箱不能为空");
        return;
    }
    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "警告", "邮箱格式不正确");
        return;
    }

    // 验证真实姓名是否为空
    if (realname.isEmpty()) {
        QMessageBox::warning(this, "警告", "真实姓名不能为空");
        return;
    }

    // 验证身份证号格式
    if (idcard.isEmpty()) {
        QMessageBox::warning(this, "警告", "身份证号不能为空");
        return;
    }
    if (idcard.length() != 18) {
        QMessageBox::warning(this, "警告", "身份证号格式不正确，请输入18位身份证号");
        return;
    }

    // 检查用户名是否已被其他用户使用 (排除当前用户)
    bool success = false;
    QString checkSql = QString("select count(1) from user_info where username='%1' AND id!=%2").arg(username).arg(m_userId);
    QSqlQuery checkQuery = m_dbOperator.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "更新失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "更新失败", "该用户名已被使用，请重新输入");
        return;
    }

    // 检查用户名是否已被管理员使用
    checkSql = QString("select count(1) from admin_info where username='%1'").arg(username);
    checkQuery = m_dbOperator.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "更新失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "更新失败", "该用户名已被使用，请重新输入");
        return;
    }

    // 检查手机号是否已被其他用户使用 (排除当前用户)
    checkSql = QString("select count(1) from user_info where phone='%1' AND id!=%2").arg(phone).arg(m_userId);
    checkQuery = m_dbOperator.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "更新失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "更新失败", "该手机号已被注册，请重新输入");
        return;
    }

    // 检查邮箱是否已被其他用户使用 (排除当前用户)
    checkSql = QString("select count(1) from user_info where email='%1' AND id!=%2").arg(email).arg(m_userId);
    checkQuery = m_dbOperator.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "更新失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "更新失败", "该邮箱已被注册，请重新输入");
        return;
    }
    
    // 检查身份证号是否已被其他用户使用 (排除当前用户)
    checkSql = QString("select count(1) from user_info where idcard='%1' AND id!=%2").arg(idcard).arg(m_userId);
    checkQuery = m_dbOperator.DBGetData(checkSql, success);
    if (!success) {
        QMessageBox::warning(this, "更新失败", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "更新失败", "该身份证号已被注册，请重新输入");
        return;
    }

    // 构建更新 SQL 语句
    QString sqlstr = QString("UPDATE user_info SET username='%1', password='%2', phone='%3', email='%4', realname='%5', idcard='%6' WHERE id=%7").arg(username).arg(password).arg(phone).arg(email).arg(realname).arg(idcard).arg(m_userId);

    // 执行更新
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);

    if (!success) {
        QMessageBox::warning(this, "更新失败", "数据库错误：" + query.lastError().text());
        return;
    }

    // 更新成功，重新加载用户信息
    loadUserInfo();
    QMessageBox::information(this, "成功", "个人信息已更新！");
}
