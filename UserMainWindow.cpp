#include "UserMainWindow.h"
#include "ui_UserMainWindow.h"

#include "FlightItemWidget.h"

#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QVector>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>

// 填充 ComboBox 函数（参数：目标 ComboBox、查询 SQL）
void UserMainWindow::fillComboBox(QComboBox *cbox, const QString &sql) {
    // 1. 清空原有选项（避免重复）
    cbox->clear();

    // 2. 执行 SQL 查询
    bool sf=false;
    QSqlQuery query = m_dbOperator.DBGetData(sql, sf);
    if (!sf) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    // 3. 遍历查询结果，添加到 ComboBox
    while (query.next()) {
        // 假设查询结果的第 0 列是要显示的内容（可根据字段名调整，如 query.value("type").toString()）
        QString itemText = query.value(0).toString();
        cbox->addItem(itemText);
    }
}

// 构造函数：初始化用户主窗口
UserMainWindow::UserMainWindow(int userId, QWidget *parent) : QMainWindow(parent), ui(new Ui::UserMainWindow), m_userId(userId) {
    ui->setupUi(this);

    m_dbOperator.DBOpen();

    ui->avatarLabel->setCursor(Qt::PointingHandCursor);
    ui->avatarLabel->installEventFilter(this);

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

    flightLayout = new QVBoxLayout(ui->scrollAreaWidgetContents_1);
    ui->scrollAreaWidgetContents_1->setLayout(flightLayout);
    flightLayout->setContentsMargins(10, 5, 10, 5);
    flightLayout->setAlignment(Qt::AlignTop);

    ui->scrollAreaWidgetContents_1->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->scrollArea_1->setWidgetResizable(false);
    ui->scrollAreaWidgetContents_1->show();

    orderLayout = new QVBoxLayout(ui->scrollAreaWidgetContents_2);
    ui->scrollAreaWidgetContents_2->setLayout(orderLayout);
    orderLayout->setContentsMargins(10, 5, 10, 5);
    orderLayout->setSpacing(15);
    orderLayout->setAlignment(Qt::AlignTop);

    ui->scrollAreaWidgetContents_2->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->scrollArea_2->setWidgetResizable(true);

    favoritesLayout = new QVBoxLayout(ui->scrollAreaWidgetContents_3);
    ui->scrollAreaWidgetContents_3->setLayout(favoritesLayout);
    favoritesLayout->setContentsMargins(10, 5, 10, 5);
    favoritesLayout->setSpacing(15);
    favoritesLayout->setAlignment(Qt::AlignTop);

    ui->scrollAreaWidgetContents_3->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents_3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->scrollArea_3->setWidgetResizable(true);

    loadAllFlights();

    if (ui->stackedWidget->currentIndex() == 1) {
        loadOrders();
    }
    if (ui->stackedWidget->currentIndex() == 2) {
        loadFavorites();
    }
    QString sql = "SELECT DISTINCT departure_city FROM flight_info";
    fillComboBox(ui->departureCbox, sql);
    sql = "SELECT DISTINCT arrival_city FROM flight_info";
    fillComboBox(ui->arrivalCbox, sql);

}

UserMainWindow::~UserMainWindow() {
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
    loadOrders();
}

// 切换到我的收藏页面
void UserMainWindow::on_myFavoritesBtn_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadFavorites();
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
        setAvatar(1);
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

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(20, 10, 20, 10);

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
    QString sqlstr = "select * from flight_info order by departure_time";
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf) {
        QMessageBox::critical(this, "加载失败", "数据库错误：" + query.lastError().text());
        return;
    }

    int viewportWidth = ui->scrollArea_1->viewport()->width();
    int contentWidth = qMax(600, viewportWidth - 20);
    ui->scrollAreaWidgetContents_1->setMinimumWidth(contentWidth);
    ui->scrollAreaWidgetContents_1->resize(contentWidth, ui->scrollAreaWidgetContents_1->height());

    // 遍历查询结果，创建航班条目
    int count = 0;
    while (query.next()) {
        count++;
        QString flightNo = query.value("flight_id").toString();

        QDateTime takeoffDateTime = query.value("departure_time").toDateTime();
        QString takeoffTime = formatDateTime(takeoffDateTime);
        QDateTime arriveDateTime = query.value("arrival_time").toDateTime();
        QString arriveTime = formatDateTime(arriveDateTime);

        QString dep = query.value("departure_airport").toString();
        QString dest = query.value("arrival_airport").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
        QString airlineCompany = query.value("airline_company").toString();

        FlightItemWidget *itemWidget = new FlightItemWidget(flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, airlineCompany, ui->scrollAreaWidgetContents_1);

        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemWidget->setFixedWidth(600);
        itemWidget->setFixedHeight(180);
        itemWidget->show();

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

    ui->scrollAreaWidgetContents_1->updateGeometry();

    ui->scrollAreaWidgetContents_1->adjustSize();
    ui->scrollAreaWidgetContents_1->update();
    ui->scrollArea_1->update();
    ui->scrollArea_1->viewport()->update();
}

// 重置查询条件并显示所有航班
void UserMainWindow::on_resetBtn_clicked() {
    ui->departureCbox->setCurrentIndex(0);
    ui->arrivalCbox->setCurrentIndex(0);
    ui->yearSpin->setValue(2026);
    ui->monthSpin->setValue(1);
    ui->daySpin->setValue(1);
    loadAllFlights();
}

// 根据条件搜索航班
void UserMainWindow::on_searchBtn_clicked() {
    clearFlightItems();

    QString departure = ui->departureCbox->currentText();
    QString destination = ui->arrivalCbox->currentText();
    QString year = ui->yearSpin->text();
    QString month = ui->monthSpin->text().rightJustified(2, '0');
    QString day = ui->daySpin->text().rightJustified(2, '0');
    QString takeoffDate = QString("%1-%2-%3").arg(year, month, day);

    bool sf = false;
    QString sqlstr = QString("select * from flight_info where departure_city='%1' and arrival_city='%2' and DATE(departure_time)='%3' order by departure_time").arg(departure, destination, takeoffDate);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, sf);
    if (!sf) {
        QMessageBox::critical(this, "查询失败", "数据库错误：" + query.lastError().text());
        return;
    }

    int viewportWidth = ui->scrollArea_1->viewport()->width();
    int contentWidth = qMax(600, viewportWidth - 20);
    ui->scrollAreaWidgetContents_1->setMinimumWidth(contentWidth);
    ui->scrollAreaWidgetContents_1->resize(contentWidth, ui->scrollAreaWidgetContents_1->height());

    int count = 0;
    while (query.next()) {
        count++;
        QString flightNo = query.value("flight_id").toString();

        QDateTime takeoffDateTime = query.value("departure_time").toDateTime();
        QString takeoffTime = formatDateTime(takeoffDateTime);
        QDateTime arriveDateTime = query.value("arrival_time").toDateTime();
        QString arriveTime = formatDateTime(arriveDateTime);

        QString dep = query.value("departure_airport").toString();
        QString dest = query.value("arrival_airport").toString();
        QString price = query.value("price").toString() + "元";
        QString remaining = QString("%1/%2").arg(query.value("remaining_seats").toString(), query.value("total_seats").toString());
        QString airlineCompany = query.value("airline_company").toString();

        FlightItemWidget *itemWidget = new FlightItemWidget(flightNo, takeoffTime, arriveTime, dep, dest, price, remaining, airlineCompany, ui->scrollAreaWidgetContents_1);

        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        itemWidget->setFixedWidth(600);
        itemWidget->setFixedHeight(180);
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

    ui->scrollAreaWidgetContents_1->updateGeometry();

    ui->scrollAreaWidgetContents_1->adjustSize();
    ui->scrollAreaWidgetContents_1->update();
    ui->scrollArea_1->update();
    ui->scrollArea_1->viewport()->update();
}

// 处理航班预订按钮点击
void UserMainWindow::on_book_clicked(const QString &flightNo) {
    // 查询航班详细信息
    bool success = false;
    QString sqlstr = QString("select * from flight_info where flight_id='%1'").arg(flightNo);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);

    if (!success || !query.next()) {
        QMessageBox::warning(this, "错误", "获取航班信息失败！");
        return;
    }

    // 获取航班信息
    QString departureAirport = query.value("departure_airport").toString();
    QString arrivalAirport = query.value("arrival_airport").toString();
    QString departureCity = query.value("departure_city").toString();
    QString arrivalCity = query.value("arrival_city").toString();
    QDateTime departureTime = query.value("departure_time").toDateTime();
    QDateTime arrivalTime = query.value("arrival_time").toDateTime();
    double price = query.value("price").toDouble();

    // 格式化时间
    QString takeoffTime = formatDateTime(departureTime);
    QString arriveTime = formatDateTime(arrivalTime);

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

    // 显示乘客信息 (使用用户信息)
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
        QString updateSql = QString("update user_info set balance=%1 where id=%2").arg(newBalance).arg(m_userId);
        QSqlQuery updateQuery = m_dbOperator.DBGetData(updateSql, updateSuccess);

        if (!updateSuccess) {
            QMessageBox::warning(bookDialog, "预订失败", "更新余额失败：" + updateQuery.lastError().text());
            return;
        }

        //生成17位order_no
        QDateTime orderTime = QDateTime::currentDateTime();
        // 查询数据库中最大的order_id，用于生成最后3位序号
        int maxOrderId = 0;
        bool isMaxIdSuccess = false;
        QSqlQuery maxIdQuery = m_dbOperator.DBGetData("select max(order_id) from order_info", isMaxIdSuccess);
        if (isMaxIdSuccess && maxIdQuery.next() && !maxIdQuery.value(0).isNull()) {
            maxOrderId = maxIdQuery.value(0).toInt() + 1; // 自增1保证唯一
        } else {
            maxOrderId = 1; // 无订单时从1开始
        }
        // 生成17位订单号：年(4)+月(2)+日(2)+时(2)+分(2)+秒(2)+序号(3)
        QString orderNo = QString("%1%2%3%4%5%6%7")
                              .arg(orderTime.date().year(), 4, 10, QChar('0'))
                              .arg(orderTime.date().month(), 2, 10, QChar('0'))
                              .arg(orderTime.date().day(), 2, 10, QChar('0'))
                              .arg(orderTime.time().hour(), 2, 10, QChar('0'))
                              .arg(orderTime.time().minute(), 2, 10, QChar('0'))
                              .arg(orderTime.time().second(), 2, 10, QChar('0'))
                              .arg(maxOrderId, 3, 10, QChar('0'));

        // 创建订单
        // 订单内保存城市维度，便于后续改签同城航线匹配
        QString insertSql = QString("insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_status) values ('%1', %2, '%3', '%4', '%5', '%6', '%7', '%8', '%9', %10, '%11')")
                                .arg(orderNo).arg(m_userId).arg(flightNo).arg(passengerName).arg(passengerIdcard)
                                .arg(departureCity).arg(arrivalCity).arg(departureTime.toString("yyyy-MM-dd hh:mm:ss"))
                                .arg(arrivalTime.toString("yyyy-MM-dd hh:mm:ss")).arg(price).arg("已支付");
        bool insertSuccess = false;
        QSqlQuery insertQuery = m_dbOperator.DBGetData(insertSql, insertSuccess);

        if (!insertSuccess) {
            // 如果订单创建失败，回滚余额
            QString rollbackSql = QString("update user_info set balance=%1 where id=%2").arg(userInfo.balance).arg(m_userId);
            m_dbOperator.DBGetData(rollbackSql, updateSuccess);
            QMessageBox::warning(bookDialog, "预订失败", "创建订单失败：" + insertQuery.lastError().text());
            return;
        }

        // 更新航班剩余座位
        int remainingSeats = query.value("remaining_seats").toInt() - 1;
        QString seatSql = QString("update flight_info set remaining_seats=%1 where flight_id='%2'").arg(remainingSeats).arg(flightNo);
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
    if (m_userId <= 0) {
        QMessageBox::warning(this, "收藏失败", "请先登录后再收藏航班");
        return;
    }

    if (m_dbOperator.isFavorite(m_userId, flightNo)) {
        QMessageBox::information(this, "提示", "该航班已在收藏夹");
        return;
    }

    if (m_dbOperator.addFavorite(m_userId, flightNo)) {
        QMessageBox::information(this, "收藏成功", "已收藏航班：" + flightNo);
        // 如果当前在收藏页，立即刷新
        if (ui->stackedWidget->currentIndex() == 2) {
            loadFavorites();
        }
    } else {
        QMessageBox::warning(this, "收藏失败", "数据库错误，收藏未成功");
    }
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
    QString sqlstr = QString("delete from user_info where id=%1").arg(m_userId);
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
    QString sqlstr = QString("update user_info set balance=%1 where id=%2").arg(newBalance).arg(m_userId);
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

// 加载订单列表（最终修正版）
void UserMainWindow::loadOrders() {
    clearOrders();

    bool success = false;
    // 添加 order_status != '已取消' 条件过滤已取消订单
    QString sqlstr = QString(
                         "select "
                         "o.order_id, o.order_no, o.user_id, o.flight_id, "
                         "o.departure_city, o.arrival_city, o.departure_time, "
                         "o.arrival_time, o.price, o.order_time, o.order_status, "
                         "u.realname as passenger_name, "
                         "u.idcard as passenger_idcard "
                         "from order_info o "
                         "inner join user_info u on o.user_id = u.id "
                         "where o.user_id=%1 and o.order_status != '已取消' "  // 过滤已取消订单
                         "order by o.order_time desc"
                         ).arg(m_userId);
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
        // 此时读取的是用户表最新数据，而非订单表旧数据
        QString passengerName = query.value("passenger_name").toString();
        QString passengerIdcard = query.value("passenger_idcard").toString();
        QString departureCity = query.value("departure_city").toString();
        QString arrivalCity = query.value("arrival_city").toString();
        QDateTime departureTime = query.value("departure_time").toDateTime();
        QDateTime arrivalTime = query.value("arrival_time").toDateTime();
        double price = query.value("price").toDouble();
        QDateTime orderTime = query.value("order_time").toDateTime();

        // 生成订单号（原有逻辑不变）
        QString orderNo = QString("%1%2%3%4%5%6%7").arg(
                                                       orderTime.date().year(), 4, 10, QChar('0')).arg(
                                  orderTime.date().month(), 2, 10, QChar('0')).arg(
                                  orderTime.date().day(), 2, 10, QChar('0')).arg(
                                  orderTime.time().hour(), 2, 10, QChar('0')).arg(
                                  orderTime.time().minute(), 2, 10, QChar('0')).arg(
                                  orderTime.time().second(), 2, 10, QChar('0')).arg(
                                  orderId, 3, 10, QChar('0'));

        // 格式化时间（原有逻辑不变）
        QString takeoffTime = formatDateTime(departureTime);
        QString arriveTime = formatDateTime(arrivalTime);

        // 创建订单显示组件（原有逻辑完全不变）
        QFrame *orderFrame = new QFrame(ui->scrollAreaWidgetContents_2);
        orderFrame->setStyleSheet("QFrame {"
                                  "    background-color: #f5f5f5;"
                                  "    border: 1px solid #ddd;"
                                  "    border-radius: 8px;"
                                  "    padding: 10px;"
                                  "}");
        orderFrame->setFixedHeight(320);
        orderFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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

        // 乘客信息（此时显示的是用户表最新数据）
        QLabel *passengerLabel = new QLabel(QString("乘客：%1 | 身份证：%2").arg(passengerName, passengerIdcard), orderFrame);
        passengerLabel->setStyleSheet("font-size: 13px;");
        orderFrameLayout->addWidget(passengerLabel);

        // 价格和下单时间
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        QLabel *priceLabel = new QLabel(QString("票价：%1 元").arg(QString::number(price, 'f', 2)), orderFrame);
        priceLabel->setStyleSheet("font-size: 13px; color: #d9534f; font-weight: bold;");
        bottomLayout->addWidget(priceLabel);

        bottomLayout->addStretch();

        QString orderTimeStr = formatDateTime(orderTime);
        QLabel *orderTimeLabel = new QLabel(QString("下单时间：%1").arg(orderTimeStr), orderFrame);
        orderTimeLabel->setStyleSheet("font-size: 13px; color: #999;");
        bottomLayout->addWidget(orderTimeLabel);

        orderFrameLayout->addLayout(bottomLayout);

        // 操作按钮（原有逻辑不变）
        QHBoxLayout *actionLayout = new QHBoxLayout();
        actionLayout->addStretch();

        QPushButton *cancelPayBtn = new QPushButton("取消支付", orderFrame);
        cancelPayBtn->setStyleSheet("QPushButton {"
                                    "    background-color: #d9534f;"
                                    "    color: white;"
                                    "    border-radius: 5px;"
                                    "    padding: 6px 20px;"
                                    "    font-size: 13px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "    background-color: #c9302c;"
                                    "}");

        actionLayout->addWidget(cancelPayBtn);

        QPushButton *rescheduleBtn = new QPushButton("改签", orderFrame);
        rescheduleBtn->setStyleSheet("QPushButton {"
                                     "    background-color: #156080;"
                                     "    color: white;"
                                     "    border-radius: 5px;"
                                     "    padding: 6px 20px;"
                                     "    font-size: 13px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "    background-color: #1a7a9f;"
                                     "}");
        actionLayout->addWidget(rescheduleBtn);

        orderFrameLayout->addLayout(actionLayout);

        connect(cancelPayBtn, &QPushButton::clicked, this, [=]() {
            handleCancelOrder(orderId, flightId, price);
        });
        connect(rescheduleBtn, &QPushButton::clicked, this, [=]() {
            handleReschedule(orderId, flightId, price, departureCity, arrivalCity);
        });

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

// 清空收藏列表
void UserMainWindow::clearFavorites() {
    while (QLayoutItem *item = favoritesLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

// 加载收藏列表
void UserMainWindow::loadFavorites() {
    clearFavorites();

    bool success = false;
    QString sqlstr = QString("select f.flight_id, f.departure_city, f.arrival_city, f.departure_airport, f.arrival_airport, f.departure_time, f.arrival_time, f.price, f.remaining_seats, f.airline_company, fav.create_time from favorite_info fav join flight_info f on f.flight_id = fav.flight_id where fav.user_id=%1 order by fav.create_time desc").arg(m_userId);
    QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);

    if (!success) {
        QMessageBox::warning(this, "加载失败", "数据库错误：" + query.lastError().text());
        return;
    }

    int count = 0;
    while (query.next()) {
        count++;
        QString flightId = query.value("flight_id").toString();
        QString depCity = query.value("departure_city").toString();
        QString arrCity = query.value("arrival_city").toString();
        QString depAirport = query.value("departure_airport").toString();
        QString arrAirport = query.value("arrival_airport").toString();
        QDateTime depTime = query.value("departure_time").toDateTime();
        QDateTime arrTime = query.value("arrival_time").toDateTime();
        double price = query.value("price").toDouble();
        int remaining = query.value("remaining_seats").toInt();
        QDateTime favTime = query.value("create_time").toDateTime();

        QString depStr = formatDateTime(depTime);
        QString arrStr = formatDateTime(arrTime);
        QString favTimeStr = favTime.isValid() ? favTime.toString("yyyy-MM-dd hh:mm") : "";

        QFrame *favFrame = new QFrame(ui->scrollAreaWidgetContents_3);
        favFrame->setStyleSheet("QFrame {"
                                "    background-color: #f5f5f5;"
                                "    border: 1px solid #ddd;"
                                "    border-radius: 8px;"
                                "    padding: 10px;"
                                "}");
        favFrame->setFixedHeight(320);

        QVBoxLayout *favLayout = new QVBoxLayout(favFrame);
        favLayout->setContentsMargins(15, 15, 15, 15);
        favLayout->setSpacing(8);

        QLabel *flightLabel = new QLabel(QString("航班号：%1 | %2 → %3").arg(flightId, depCity, arrCity), favFrame);
        flightLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #156080;");
        favLayout->addWidget(flightLabel);

        QLabel *airportLabel = new QLabel(QString("机场：%1 → %2").arg(depAirport, arrAirport), favFrame);
        airportLabel->setStyleSheet("font-size: 13px;");
        favLayout->addWidget(airportLabel);

        QLabel *timeLabel = new QLabel(QString("起飞：%1 | 到达：%2").arg(depStr, arrStr), favFrame);
        timeLabel->setStyleSheet("font-size: 13px;");
        favLayout->addWidget(timeLabel);

        QLabel *priceLabel = new QLabel(QString("票价：%1 元 | 剩余座位：%2").arg(QString::number(price, 'f', 2)).arg(remaining), favFrame);
        priceLabel->setStyleSheet("font-size: 13px; color: #d9534f; font-weight: bold;");
        favLayout->addWidget(priceLabel);

        if (!favTimeStr.isEmpty()) {
            QLabel *favTimeLabel = new QLabel(QString("收藏时间：%1").arg(favTimeStr), favFrame);
            favTimeLabel->setStyleSheet("font-size: 12px; color: #777;");
            favLayout->addWidget(favTimeLabel);
        }

        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch();

        QPushButton *payBtn = new QPushButton("去支付", favFrame);
        payBtn->setStyleSheet("QPushButton {"
                              "    background-color: #156080;"
                              "    color: white;"
                              "    border-radius: 5px;"
                              "    padding: 6px 20px;"
                              "    font-size: 13px;"
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #1a7a9f;"
                              "}");
        btnLayout->addWidget(payBtn);

        QPushButton *cancelFavBtn = new QPushButton("取消收藏", favFrame);
        cancelFavBtn->setStyleSheet("QPushButton {"
                                    "    background-color: #d9534f;"
                                    "    color: white;"
                                    "    border-radius: 5px;"
                                    "    padding: 6px 20px;"
                                    "    font-size: 13px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "    background-color: #c9302c;"
                                    "}");
        btnLayout->addWidget(cancelFavBtn);

        favLayout->addLayout(btnLayout);

        connect(payBtn, &QPushButton::clicked, this, [=]() {
            on_book_clicked(flightId);
        });
        connect(cancelFavBtn, &QPushButton::clicked, this, [=]() {
            handleUnfavorite(flightId);
        });

        favoritesLayout->addWidget(favFrame);
    }

    if (count == 0) {
        QLabel *noFavLabel = new QLabel("暂无收藏", ui->scrollAreaWidgetContents_3);
        noFavLabel->setStyleSheet("font-size: 16px; color: #999; padding: 50px;");
        noFavLabel->setAlignment(Qt::AlignCenter);
        favoritesLayout->addWidget(noFavLabel);
    }

    favoritesLayout->addStretch();
}

// 取消收藏
void UserMainWindow::handleUnfavorite(const QString &flightId) {
    if (m_dbOperator.removeFavorite(m_userId, flightId)) {
        QMessageBox::information(this, "已取消收藏", "已取消收藏航班：" + flightId);
        loadFavorites();
    } else {
        QMessageBox::warning(this, "操作失败", "取消收藏失败，请重试");
    }
}

// 取消已支付订单：退还余额并释放座位
void UserMainWindow::handleCancelOrder(int orderId, const QString &flightId, double price) {
    // 1. 显示确认对话框
    int ret = QMessageBox::question(this, "确认取消",
                                    "确定要取消此订单吗？\n取消后将退款至您的账户。",
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

     // 2. 开始数据库操作
    bool success = false;

    // 2.1 更新订单状态为"已取消"
    QString updateOrderSql = QString("update order_info set order_status='已取消' where order_id=%1").arg(orderId);
    QSqlQuery orderQuery = m_dbOperator.DBGetData(updateOrderSql, success);
    if (!success) {
        QMessageBox::warning(this, "操作失败", "更新订单状态失败：" + orderQuery.lastError().text());
        return;
    }

    // 2.2 恢复航班剩余座位
    QString flightSql = QString("select remaining_seats from flight_info where flight_id='%1'").arg(flightId);
    QSqlQuery flightQuery = m_dbOperator.DBGetData(flightSql, success);
    if (!success || !flightQuery.next()) {
        QMessageBox::warning(this, "操作失败", "获取航班信息失败：" + flightQuery.lastError().text());
        return;
    }
    int remainingSeats = flightQuery.value("remaining_seats").toInt() + 1;
    QString updateFlightSql = QString("update flight_info set remaining_seats=%1 where flight_id='%2'")
                                  .arg(remainingSeats).arg(flightId);
    m_dbOperator.DBGetData(updateFlightSql, success);

    // 2.3 退还订单金额到用户账户
    DBOperator::UserInfo userInfo;
    if (m_dbOperator.getUserInfo(m_userId, userInfo)) {
        double newBalance = userInfo.balance + price;
        QString updateBalanceSql = QString("update user_info set balance=%1 where id=%2")
                                       .arg(newBalance).arg(m_userId);
        m_dbOperator.DBGetData(updateBalanceSql, success);

        // 更新界面显示的余额
        loadUserInfo();
    }

    // 3. 重新加载订单列表（会自动过滤已取消订单）
    loadOrders();
    QMessageBox::information(this, "操作成功", "订单已取消，金额已退还至您的账户");
}

// 改签订单：选择新航班后差额结算并更新订单
void UserMainWindow::handleReschedule(int orderId, const QString &oldFlightId, double oldPrice, const QString &departureCity, const QString &arrivalCity) {
    struct FlightOption {
        QString flightId;
        QString depAirport;
        QString arrAirport;
        QDateTime depTime;
        QDateTime arrTime;
        double price;
        int remaining;
        QString airline;
    };

    QVector<FlightOption> options;
    auto refreshFlights = [&](const QString &depCity, const QString &arrCity, const QString &depAirport = "", const QString &arrAirport = "", const QDate &targetDate = QDate()) -> bool {
        options.clear();
        bool success = false;
        QString sqlstr = QString("select flight_id, departure_airport, arrival_airport, departure_time, arrival_time, price, remaining_seats, airline_company from flight_info where departure_city='%1' and arrival_city='%2' and remaining_seats>0 and flight_id!='%3'").arg(depCity, arrCity, oldFlightId);
        
        if (!depAirport.isEmpty()) {
            sqlstr += QString(" and departure_airport='%1'").arg(depAirport);
        }
        if (!arrAirport.isEmpty()) {
            sqlstr += QString(" and arrival_airport='%1'").arg(arrAirport);
        }
        if (targetDate.isValid()) {
            sqlstr += QString(" and date(departure_time)='%1'").arg(targetDate.toString("yyyy-MM-dd"));
        }
        
        sqlstr += " order by departure_time";
        
        QSqlQuery query = m_dbOperator.DBGetData(sqlstr, success);
        if (!success) {
            return false;
        }

        while (query.next()) {
            FlightOption opt;
            opt.flightId = query.value("flight_id").toString();
            opt.depAirport = query.value("departure_airport").toString();
            opt.arrAirport = query.value("arrival_airport").toString();
            opt.depTime = query.value("departure_time").toDateTime();
            opt.arrTime = query.value("arrival_time").toDateTime();
            opt.price = query.value("price").toDouble();
            opt.remaining = query.value("remaining_seats").toInt();
            opt.airline = query.value("airline_company").toString();
            options.append(opt);
        }
        return true;
    };

    if (!refreshFlights(departureCity, arrivalCity)) {
        QMessageBox::warning(this, "加载失败", "查询可改签航班失败。");
        return;
    }

    QDialog *filterDialog = new QDialog(this);
    filterDialog->setWindowTitle("改签筛选条件");
    filterDialog->setModal(true);
    filterDialog->resize(480, 400);

    QVBoxLayout *filterLayout = new QVBoxLayout(filterDialog);
    
    QLabel *titleLabel = new QLabel("设置改签筛选条件：", filterDialog);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px 0;");
    filterLayout->addWidget(titleLabel);

    QGridLayout *gridLayout = new QGridLayout();
    
    QLabel *depCityLabel = new QLabel("出发城市：", filterDialog);
    QComboBox *depCityCombo = new QComboBox(filterDialog);
    bool success = false;
    QSqlQuery cityQuery = m_dbOperator.DBGetData("select distinct departure_city from flight_info order by departure_city", success);
    if (success) {
        while (cityQuery.next()) {
            QString city = cityQuery.value(0).toString();
            depCityCombo->addItem(city);
            if (city == departureCity) {
                depCityCombo->setCurrentIndex(depCityCombo->count() - 1);
            }
        }
    }
    gridLayout->addWidget(depCityLabel, 0, 0);
    gridLayout->addWidget(depCityCombo, 0, 1);

    QLabel *arrCityLabel = new QLabel("到达城市：", filterDialog);
    QComboBox *arrCityCombo = new QComboBox(filterDialog);
    cityQuery = m_dbOperator.DBGetData("select distinct arrival_city from flight_info order by arrival_city", success);
    if (success) {
        while (cityQuery.next()) {
            QString city = cityQuery.value(0).toString();
            arrCityCombo->addItem(city);
            if (city == arrivalCity) {
                arrCityCombo->setCurrentIndex(arrCityCombo->count() - 1);
            }
        }
    }
    gridLayout->addWidget(arrCityLabel, 1, 0);
    gridLayout->addWidget(arrCityCombo, 1, 1);

    QLabel *depAirportLabel = new QLabel("出发机场：", filterDialog);
    QComboBox *depAirportCombo = new QComboBox(filterDialog);
    depAirportCombo->addItem("（不限）");
    QCheckBox *depAirportCheck = new QCheckBox("启用", filterDialog);
    depAirportCheck->setChecked(false);
    depAirportCombo->setEnabled(false);
    gridLayout->addWidget(depAirportLabel, 2, 0);
    gridLayout->addWidget(depAirportCombo, 2, 1);
    gridLayout->addWidget(depAirportCheck, 2, 2);

    QLabel *arrAirportLabel = new QLabel("到达机场：", filterDialog);
    QComboBox *arrAirportCombo = new QComboBox(filterDialog);
    arrAirportCombo->addItem("（不限）");
    QCheckBox *arrAirportCheck = new QCheckBox("启用", filterDialog);
    arrAirportCheck->setChecked(false);
    arrAirportCombo->setEnabled(false);
    gridLayout->addWidget(arrAirportLabel, 3, 0);
    gridLayout->addWidget(arrAirportCombo, 3, 1);
    gridLayout->addWidget(arrAirportCheck, 3, 2);

    QLabel *dateLabel = new QLabel("出发日期：", filterDialog);
    QDateEdit *dateEdit = new QDateEdit(filterDialog);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumDate(QDate::currentDate());
    QCheckBox *dateCheck = new QCheckBox("启用", filterDialog);
    dateCheck->setChecked(false);
    dateEdit->setEnabled(false);
    gridLayout->addWidget(dateLabel, 4, 0);
    gridLayout->addWidget(dateEdit, 4, 1);
    gridLayout->addWidget(dateCheck, 4, 2);

    auto updateAirportOptions = [&](QComboBox *combo, const QString &city, bool isDeparture) {
        combo->clear();
        combo->addItem("（不限）");
        QString sql = isDeparture ? 
            QString("select distinct departure_airport from flight_info where departure_city='%1' order by departure_airport").arg(city) :
            QString("select distinct arrival_airport from flight_info where arrival_city='%1' order by arrival_airport").arg(city);
        QSqlQuery query = m_dbOperator.DBGetData(sql, success);
        if (success) {
            while (query.next()) {
                combo->addItem(query.value(0).toString());
            }
        }
    };

    connect(depCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        if (depAirportCheck->isChecked()) {
            updateAirportOptions(depAirportCombo, depCityCombo->currentText(), true);
        }
    });

    connect(arrCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        if (arrAirportCheck->isChecked()) {
            updateAirportOptions(arrAirportCombo, arrCityCombo->currentText(), false);
        }
    });

    connect(depAirportCheck, &QCheckBox::toggled, [&](bool checked) {
        depAirportCombo->setEnabled(checked);
        if (checked) {
            updateAirportOptions(depAirportCombo, depCityCombo->currentText(), true);
        }
    });

    connect(arrAirportCheck, &QCheckBox::toggled, [&](bool checked) {
        arrAirportCombo->setEnabled(checked);
        if (checked) {
            updateAirportOptions(arrAirportCombo, arrCityCombo->currentText(), false);
        }
    });

    connect(dateCheck, &QCheckBox::toggled, dateEdit, &QDateEdit::setEnabled);

    filterLayout->addLayout(gridLayout);

    QLabel *resultLabel = new QLabel(QString("找到 %1 个可改签航班").arg(options.size()), filterDialog);
    resultLabel->setStyleSheet("font-size: 12px; color: #666; padding: 10px 0;");
    filterLayout->addWidget(resultLabel);

    QHBoxLayout *filterBtnLayout = new QHBoxLayout();
    filterBtnLayout->addStretch();
    QPushButton *searchBtn = new QPushButton("查询", filterDialog);
    searchBtn->setStyleSheet("QPushButton { background-color: #156080; color: white; border-radius: 5px; padding: 8px 24px; font-size: 13px; } QPushButton:hover { background-color: #1a7a9f; }");
    QPushButton *cancelFilterBtn = new QPushButton("取消", filterDialog);
    cancelFilterBtn->setStyleSheet("QPushButton { background-color: #999; color: white; border-radius: 5px; padding: 8px 24px; font-size: 13px; } QPushButton:hover { background-color: #777; }");
    filterBtnLayout->addWidget(searchBtn);
    filterBtnLayout->addWidget(cancelFilterBtn);
    filterLayout->addLayout(filterBtnLayout);

    QString selectedDepCity = departureCity;
    QString selectedArrCity = arrivalCity;

    connect(cancelFilterBtn, &QPushButton::clicked, filterDialog, &QDialog::reject);
    connect(searchBtn, &QPushButton::clicked, [&, depCityCombo, arrCityCombo, depAirportCheck, depAirportCombo, arrAirportCheck, arrAirportCombo, dateCheck, dateEdit, resultLabel, filterDialog]() {
        selectedDepCity = depCityCombo->currentText();
        selectedArrCity = arrCityCombo->currentText();
        QString depAirport = depAirportCheck->isChecked() && depAirportCombo->currentIndex() > 0 ? depAirportCombo->currentText() : "";
        QString arrAirport = arrAirportCheck->isChecked() && arrAirportCombo->currentIndex() > 0 ? arrAirportCombo->currentText() : "";
        QDate targetDate = dateCheck->isChecked() ? dateEdit->date() : QDate();
        
        if (!refreshFlights(selectedDepCity, selectedArrCity, depAirport, arrAirport, targetDate)) {
            QMessageBox::warning(filterDialog, "查询失败", "查询可改签航班失败。");
            return;
        }
        
        resultLabel->setText(QString("找到 %1 个可改签航班").arg(options.size()));
        if (options.isEmpty()) {
            QMessageBox::information(filterDialog, "无结果", "根据筛选条件未找到可改签的航班。");
        } else {
            filterDialog->accept();
        }
    });

    if (filterDialog->exec() != QDialog::Accepted || options.isEmpty()) {
        filterDialog->deleteLater();
        if (options.isEmpty()) {
            QMessageBox::information(this, "无可改签航班", "当前筛选条件下暂无可改签的航班。");
        }
        return;
    }

    filterDialog->deleteLater();

    QString finalDepCity = selectedDepCity;
    QString finalArrCity = selectedArrCity;

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择改签航班");
    dialog->setModal(true);
    dialog->resize(520, 220);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QLabel *tip = new QLabel("请选择要改签的航班：", dialog);
    tip->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px 0;");
    mainLayout->addWidget(tip);

    QComboBox *combo = new QComboBox(dialog);
    for (const auto &opt : options) {
        QString depTimeStr = QString("%1-%2-%3 %4:%5").arg(opt.depTime.date().year()).arg(opt.depTime.date().month(), 2, 10, QChar('0')).arg(opt.depTime.date().day(), 2, 10, QChar('0')).arg(opt.depTime.time().hour(), 2, 10, QChar('0')).arg(opt.depTime.time().minute(), 2, 10, QChar('0'));
        QString arrTimeStr = QString("%1-%2-%3 %4:%5").arg(opt.arrTime.date().year()).arg(opt.arrTime.date().month(), 2, 10, QChar('0')).arg(opt.arrTime.date().day(), 2, 10, QChar('0')).arg(opt.arrTime.time().hour(), 2, 10, QChar('0')).arg(opt.arrTime.time().minute(), 2, 10, QChar('0'));
        QString text = QString("%1 | %2 → %3 | 起飞 %4 | 票价 %5 元 | 剩余 %6").arg(opt.flightId, opt.depAirport, opt.arrAirport, depTimeStr).arg(QString::number(opt.price, 'f', 2)).arg(opt.remaining);
        combo->addItem(text);
    }
    mainLayout->addWidget(combo);

    QLabel *balanceTip = new QLabel(QString("当前订单票价：%1 元").arg(QString::number(oldPrice, 'f', 2)), dialog);
    balanceTip->setStyleSheet("font-size: 12px; color: #666; padding: 5px 0;");
    mainLayout->addWidget(balanceTip);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton *okBtn = new QPushButton("确认改签", dialog);
    okBtn->setStyleSheet("QPushButton {"
                         "    background-color: #156080;"
                         "    color: white;"
                         "    border-radius: 5px;"
                         "    padding: 8px 24px;"
                         "    font-size: 13px;"
                         "}"
                         "QPushButton:hover {"
                         "    background-color: #1a7a9f;"
                         "}");
    QPushButton *cancelBtn = new QPushButton("取消", dialog);
    cancelBtn->setStyleSheet("QPushButton {"
                             "    background-color: #999;"
                             "    color: white;"
                             "    border-radius: 5px;"
                             "    padding: 8px 24px;"
                             "    font-size: 13px;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #777;"
                             "}");
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    connect(okBtn, &QPushButton::clicked, dialog, [=]() {
        int idx = combo->currentIndex();
        if (idx < 0 || idx >= options.size()) {
            QMessageBox::warning(dialog, "改签失败", "请选择改签航班。");
            return;
        }

        FlightOption opt = options.at(idx);

        DBOperator::UserInfo userInfo;
        if (!m_dbOperator.getUserInfo(m_userId, userInfo)) {
            QMessageBox::warning(dialog, "改签失败", "获取用户信息失败。");
            return;
        }

        double available = userInfo.balance + oldPrice;
        if (available < opt.price) {
            QMessageBox::warning(dialog, "余额不足", QString("改签后需支付 %1 元，当前余额不足。").arg(QString::number(opt.price, 'f', 2)));
            return;
        }

        // 更新余额 (退旧购新)
        double newBalance = available - opt.price;
        bool updateOk = false;
        QString balanceSql = QString("update user_info set balance=%1 where id=%2").arg(newBalance).arg(m_userId);
        QSqlQuery balanceQuery = m_dbOperator.DBGetData(balanceSql, updateOk);
        if (!updateOk) {
            QMessageBox::warning(dialog, "改签失败", "更新余额失败：" + balanceQuery.lastError().text());
            return;
        }

        // 释放旧航班座位
        QString releaseSql = QString("update flight_info set remaining_seats=remaining_seats+1 where flight_id='%1'").arg(oldFlightId);
        QSqlQuery releaseQuery = m_dbOperator.DBGetData(releaseSql, updateOk);
        if (!updateOk || releaseQuery.numRowsAffected() == 0) {
            QMessageBox::warning(dialog, "改签失败", "释放旧航班座位失败：" + releaseQuery.lastError().text());
            return;
        }

        // 占用新航班座位
        QString takeSql = QString("update flight_info set remaining_seats=remaining_seats-1 where flight_id='%1' and remaining_seats>0").arg(opt.flightId);
        QSqlQuery takeQuery = m_dbOperator.DBGetData(takeSql, updateOk);
        if (!updateOk || takeQuery.numRowsAffected() == 0) {
            QMessageBox::warning(dialog, "改签失败", "占用新航班座位失败：" + takeQuery.lastError().text());
            return;
        }

        // 更新订单信息
        QString updateOrderSql = QString("update order_info set flight_id='%1', departure_city='%2', arrival_city='%3', departure_time='%4', arrival_time='%5', price=%6, order_time=now() where order_id=%7").arg(opt.flightId).arg(finalDepCity).arg(finalArrCity).arg(opt.depTime.toString("yyyy-MM-dd hh:mm:ss")).arg(opt.arrTime.toString("yyyy-MM-dd hh:mm:ss")).arg(opt.price).arg(orderId);
        QSqlQuery updateOrderQuery = m_dbOperator.DBGetData(updateOrderSql, updateOk);
        if (!updateOk) {
            QMessageBox::warning(dialog, "改签失败", "更新订单失败：" + updateOrderQuery.lastError().text());
            return;
        }

        loadUserInfo();
        loadOrders();
        QMessageBox::information(dialog, "改签成功", "改签成功！余额已自动结算。");
        dialog->accept();
    });

    dialog->exec();
    dialog->deleteLater();
}

// 进入编辑模式
void UserMainWindow::enterEditMode() {
    if (m_isEditMode) {
        return;
    }

    m_isEditMode = true;

    m_dbOperator.getUserInfo(m_userId, m_originalUserInfo);

    QRect usernameRect = ui->UsernameLabel->geometry();
    QRect passwordRect = ui->PasswordLabel->geometry();
    QRect phoneRect = ui->PhoneLabel->geometry();
    QRect emailRect = ui->EmailLabel->geometry();
    QRect realnameRect = ui->RealnameLabel->geometry();
    QRect idcardRect = ui->IdcardLabel->geometry();

    m_usernameEdit = new QLineEdit(ui->page_4);
    m_usernameEdit->setGeometry(usernameRect);
    m_usernameEdit->setText(ui->UsernameLabel->text());
    m_usernameEdit->setFont(ui->UsernameLabel->font());
    m_usernameEdit->show();
    ui->UsernameLabel->hide();

    m_passwordEdit = new QLineEdit(ui->page_4);
    m_passwordEdit->setGeometry(passwordRect);
    m_passwordEdit->setText(ui->PasswordLabel->text());
    m_passwordEdit->setFont(ui->PasswordLabel->font());
    m_passwordEdit->setEchoMode(QLineEdit::Normal);
    m_passwordEdit->show();
    ui->PasswordLabel->hide();

    m_phoneEdit = new QLineEdit(ui->page_4);
    m_phoneEdit->setGeometry(phoneRect);
    m_phoneEdit->setText(ui->PhoneLabel->text());
    m_phoneEdit->setFont(ui->PhoneLabel->font());
    m_phoneEdit->show();
    ui->PhoneLabel->hide();

    m_emailEdit = new QLineEdit(ui->page_4);
    m_emailEdit->setGeometry(emailRect);
    m_emailEdit->setText(ui->EmailLabel->text());
    m_emailEdit->setFont(ui->EmailLabel->font());
    m_emailEdit->show();
    ui->EmailLabel->hide();

    m_realnameEdit = new QLineEdit(ui->page_4);
    m_realnameEdit->setGeometry(realnameRect);
    m_realnameEdit->setText(ui->RealnameLabel->text());
    m_realnameEdit->setFont(ui->RealnameLabel->font());
    m_realnameEdit->show();
    ui->RealnameLabel->hide();

    m_idcardEdit = new QLineEdit(ui->page_4);
    m_idcardEdit->setGeometry(idcardRect);
    m_idcardEdit->setText(ui->IdcardLabel->text());
    m_idcardEdit->setFont(ui->IdcardLabel->font());
    m_idcardEdit->show();
    ui->IdcardLabel->hide();

    ui->editInfoBtn->hide();
    ui->rechargeBtn->hide();
    ui->cancelAccountBtn->hide();

    m_saveBtn = new QPushButton(ui->page_4);
    QRect saveBtnRect = ui->editInfoBtn->geometry();
    saveBtnRect.moveLeft(saveBtnRect.x() + 105);
    m_saveBtn->setGeometry(saveBtnRect);
    m_saveBtn->setText("保存");
    m_saveBtn->setFont(ui->editInfoBtn->font());
    m_saveBtn->setStyleSheet(ui->editInfoBtn->styleSheet());
    m_saveBtn->show();
    connect(m_saveBtn, &QPushButton::clicked, this, &UserMainWindow::on_saveBtn_clicked);

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

// 退出编辑模式
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
    QString checkSql = QString("select count(1) from user_info where username='%1' and id!=%2").arg(username).arg(m_userId);
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
    checkSql = QString("select count(1) from user_info where phone='%1' and id!=%2").arg(phone).arg(m_userId);
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
    checkSql = QString("select count(1) from user_info where email='%1' and id!=%2").arg(email).arg(m_userId);
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
    checkSql = QString("select count(1) from user_info where idcard='%1' and id!=%2").arg(idcard).arg(m_userId);
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
    QString sqlstr = QString("update user_info set username='%1', password='%2', phone='%3', email='%4', realname='%5', idcard='%6' where id=%7").arg(username).arg(password).arg(phone).arg(email).arg(realname).arg(idcard).arg(m_userId);

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

// 格式化日期时间为中文格式
QString UserMainWindow::formatDateTime(const QDateTime& dateTime) {
    return QString("%1年%2月%3日 %4:%5")
        .arg(dateTime.date().year())
        .arg(dateTime.date().month())
        .arg(dateTime.date().day())
        .arg(dateTime.time().hour(), 2, 10, QChar('0'))
        .arg(dateTime.time().minute(), 2, 10, QChar('0'));
}
