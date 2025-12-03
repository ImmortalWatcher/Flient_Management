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
    DBOperator();
    void DBOpen();
    void DBClose();
    QSqlQuery DBGetData(QString sqlstr, bool &sucessFlag);
    int getUserAvatarId(int userId);
    bool updateUserAvatarId(int userId, int avatarId);
};

#endif // DBOPERATOR_H
