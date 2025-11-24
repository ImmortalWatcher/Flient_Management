/********************************************************************************
** Form generated from reading UI file 'LoginDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDLG_H
#define UI_LOGINDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDlg
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *loginBtn;
    QLabel *label;
    QLineEdit *AccountEdit;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer;
    QLineEdit *PasswordEdit;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_5;

    void setupUi(QDialog *LoginDlg)
    {
        if (LoginDlg->objectName().isEmpty())
            LoginDlg->setObjectName("LoginDlg");
        LoginDlg->resize(553, 400);
        widget = new QWidget(LoginDlg);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(32, 14, 496, 355));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(108, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 3, 0, 3, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        gridLayout->addItem(verticalSpacer_3, 2, 1, 1, 3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 7, 1, 1, 1);

        loginBtn = new QPushButton(widget);
        loginBtn->setObjectName("loginBtn");
        loginBtn->setEnabled(true);
        loginBtn->setMinimumSize(QSize(150, 40));
        loginBtn->setMaximumSize(QSize(150, 40));
        QFont font;
        font.setPointSize(20);
        loginBtn->setFont(font);

        gridLayout->addWidget(loginBtn, 7, 2, 1, 1);

        label = new QLabel(widget);
        label->setObjectName("label");
        label->setFont(font);

        gridLayout->addWidget(label, 1, 1, 1, 3);

        AccountEdit = new QLineEdit(widget);
        AccountEdit->setObjectName("AccountEdit");
        AccountEdit->setMinimumSize(QSize(250, 35));
        AccountEdit->setMaximumSize(QSize(250, 35));

        gridLayout->addWidget(AccountEdit, 3, 1, 1, 3);

        verticalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        gridLayout->addItem(verticalSpacer_4, 6, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(17, 98, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 8, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 58, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        gridLayout->addItem(verticalSpacer, 0, 2, 1, 1);

        PasswordEdit = new QLineEdit(widget);
        PasswordEdit->setObjectName("PasswordEdit");
        PasswordEdit->setMinimumSize(QSize(250, 35));
        PasswordEdit->setMaximumSize(QSize(250, 35));
        PasswordEdit->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(PasswordEdit, 5, 1, 1, 3);

        horizontalSpacer_4 = new QSpacerItem(128, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 3, 4, 3, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 7, 3, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 15, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        gridLayout->addItem(verticalSpacer_5, 4, 2, 1, 1);


        retranslateUi(LoginDlg);

        QMetaObject::connectSlotsByName(LoginDlg);
    } // setupUi

    void retranslateUi(QDialog *LoginDlg)
    {
        LoginDlg->setWindowTitle(QCoreApplication::translate("LoginDlg", "Dialog", nullptr));
        loginBtn->setText(QCoreApplication::translate("LoginDlg", "\347\231\273\345\275\225", nullptr));
        label->setText(QCoreApplication::translate("LoginDlg", "\346\254\242\350\277\216\344\275\277\347\224\250\350\210\252\347\217\255\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        AccountEdit->setPlaceholderText(QCoreApplication::translate("LoginDlg", "\350\257\267\350\276\223\345\205\245\350\264\246\345\217\267", nullptr));
        PasswordEdit->setPlaceholderText(QCoreApplication::translate("LoginDlg", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDlg: public Ui_LoginDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDLG_H
