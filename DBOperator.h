#ifndef DBOPERATOR_H
#define DBOPERATOR_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class DBOperator {
private:
    QSqlDatabase dbcon;
    bool openFlag;

public:
    struct UserInfo {
        QString username;
        QString password;
        QString phone;
        QString email;
        QString realname;
        QString idcard;
        int avatarId;
    };

    DBOperator();
    void DBOpen();
    void DBClose();

    QSqlQuery DBGetData(QString sqlstr, bool &sucessFlag);
    bool getUserInfo(int userId, UserInfo &userInfo);
    bool updateUserAvatarId(int userId, int avatarId);
};

#endif // DBOPERATOR_H
