#include "DBOperator.h"

DBOperator::DBOperator() {
    openFlag = false;
}

void DBOperator::DBOpen() {
    if(!openFlag) {
        dbcon = QSqlDatabase::addDatabase("QODBC");
        dbcon.setHostName("127.0.0.1");
        dbcon.setPort(3306);
        dbcon.setDatabaseName("flient_managementODBC");
        bool ok = dbcon.open();
        if(!ok) {
            qDebug() << "Error, flient_managementODBC 数据库文件打开失败！";
        } else {
            qDebug() << "Sucess, flient_managementODBC 数据库文件成功打开！";
            openFlag = true;
        }
    }
}

/*
void DBOperator::DBOpen() {
    QString dsn;
    if(!openFlag) {
        dbcon = QSqlDatabase::addDatabase("QODBC");
        dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access}; DBQ=C:/qtsrc/SQLTest/SQLDB.mdb");
        dbcon.setDatabaseName(dsn);
        bool ok = dbcon.open();
        if(!ok) qDebug() << "错误, SQLDB 数据库文件打开失败！";
        openFlag = true;
    }
}
*/

void DBOperator::DBClose() {
    openFlag = false;
    dbcon.close();
}

QSqlQuery DBOperator::DBGetData(QString sqlstr, bool &sucessFlag) {
    QSqlQuery query = QSqlQuery(dbcon);
    sucessFlag = query.exec(sqlstr);
    return query;
}

// 获取用户完整信息
bool DBOperator::getUserInfo(int userId, UserInfo &userInfo) {
    bool sf = false;
    QString sqlstr = QString("select username, password, phone, email, realname, idcard, avatarid from user_info where id=%1").arg(userId);
    QSqlQuery qs = DBGetData(sqlstr, sf);

    if (sf && qs.next()) {
        userInfo.username = qs.value("username").toString();
        userInfo.password = qs.value("password").toString();
        userInfo.phone = qs.value("phone").toString();
        userInfo.email = qs.value("email").toString();
        userInfo.realname = qs.value("realname").toString();
        userInfo.idcard = qs.value("idcard").toString();
        userInfo.avatarId = qs.value("avatarid").toInt();
        return true;
    }
    return false;
}

// 更新用户头像编号
bool DBOperator::updateUserAvatarId(int userId, int avatarId) {
    bool sf = false;
    QString sqlstr = QString("update user_info set avatarid=%1 where id=%2").arg(avatarId).arg(userId);
    QSqlQuery qs = DBGetData(sqlstr, sf);
    return sf;
}
