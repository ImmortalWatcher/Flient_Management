#include "DBOperator.h"

// 构造函数：初始化数据库连接标志并设置唯一连接名
DBOperator::DBOperator() : openFlag(false) {
    connectionName = QString("conn_%1").arg(reinterpret_cast<quintptr>(this));
}

// 打开数据库连接
void DBOperator::DBOpen() {
    if (!openFlag) {
        // 为每个实例创建独立连接，避免被其他实例关闭
        if (QSqlDatabase::contains(connectionName)) {
            dbcon = QSqlDatabase::database(connectionName);
        } else {
            dbcon = QSqlDatabase::addDatabase("QODBC", connectionName);
        }
        dbcon.setHostName("127.0.0.1");
        dbcon.setPort(3306);
        dbcon.setDatabaseName("flient_managementODBC");
        bool ok = dbcon.open();
        if (!ok) {
            qDebug() << "Error, flient_managementODBC 数据库文件打开失败！";
        } else {
            qDebug() << "Success, flient_managementODBC 数据库文件成功打开！";
            openFlag = true;
        }
    }
}

// 关闭数据库连接
void DBOperator::DBClose() {
    openFlag = false;
    dbcon.close();
}

// 执行 SQL 查询并返回结果
QSqlQuery DBOperator::DBGetData(QString sqlstr, bool &sucessFlag) {
    QSqlQuery query = QSqlQuery(dbcon);
    sucessFlag = query.exec(sqlstr);
    return query;
}

// 根据用户 ID 获取用户完整信息
bool DBOperator::getUserInfo(int userId, UserInfo &userInfo) {
    bool sf = false;
    QString sqlstr = QString("select username, password, phone, email, realname, idcard, avatarid, COALESCE(balance, 0) as balance from user_info where id=%1").arg(userId);
    QSqlQuery qs = DBGetData(sqlstr, sf);

    // 如果查询成功，填充用户信息结构体
    if (sf && qs.next()) {
        userInfo.username = qs.value("username").toString();
        userInfo.password = qs.value("password").toString();
        userInfo.phone = qs.value("phone").toString();
        userInfo.email = qs.value("email").toString();
        userInfo.realname = qs.value("realname").toString();
        userInfo.idcard = qs.value("idcard").toString();
        userInfo.balance = qs.value("balance").toDouble();
        userInfo.avatarId = qs.value("avatarid").toInt();
        return true;
    }
    return false;
}

// 更新用户头像 ID
bool DBOperator::updateUserAvatarId(int userId, int avatarId) {
    bool sf = false;
    QString sqlstr = QString("UPDATE user_info SET avatarid=%1 where id=%2").arg(avatarId).arg(userId);
    QSqlQuery qs = DBGetData(sqlstr, sf);
    return sf;
}
