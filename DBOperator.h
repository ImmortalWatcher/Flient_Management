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
};

#endif // DBOPERATOR_H
