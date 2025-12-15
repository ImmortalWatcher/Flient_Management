#ifndef DBOPERATOR_H
#define DBOPERATOR_H

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class DBOperator {
private:
    QSqlDatabase dbcon;
    bool openFlag;
    QString connectionName;

public:
    struct UserInfo {
        QString username;
        QString password;
        QString phone;
        QString email;
        QString realname;
        QString idcard;
        double balance;
        int avatarId;
    };

    // 构造函数和连接管理
    DBOperator();
    void DBOpen();
    void DBClose();

    // 数据库操作
    QSqlQuery DBGetData(QString sqlstr, bool &sucessFlag);

    // 用户信息相关
    bool getUserInfo(int userId, UserInfo &userInfo);
    bool updateUserAvatarId(int userId, int avatarId);

    // 收藏相关
    bool addFavorite(int userId, const QString& flightId);
    bool removeFavorite(int userId, const QString& flightId);
    bool isFavorite(int userId, const QString& flightId);
};

#endif // DBOPERATOR_H
