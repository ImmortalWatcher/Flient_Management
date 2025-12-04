#include "UserMainWindow.h"
#include "ui_UserMainWindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QGridLayout>

UserMainWindow::UserMainWindow(int userId, QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::UserMainWindow), m_userId(userId) {
    ui->setupUi(this);

    m_dbOperator.DBOpen();

    // 设置头像标签为可点击
    ui->avatarLabel->setCursor(Qt::PointingHandCursor);
    ui->avatarLabel->installEventFilter(this);

    // 加载头像
    loadAvatar();
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
    int avatarId = m_dbOperator.getUserAvatarId(m_userId);
    setAvatar(avatarId);
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
