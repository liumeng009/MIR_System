#ifndef SQLITEUTIL_H
#define SQLITEUTIL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include <QVariant>
#include <QSqlQueryModel>
#include <QDebug>

class SqliteUtil : public QObject
{
    Q_OBJECT
public:
    explicit SqliteUtil(QObject *parent = nullptr,
                        QString strDatabase = "my.db");
    ~SqliteUtil();

    bool createConnection(const QString& strConn);
    bool createTable(const QString &strQuery);

    QSqlRecord ExecuteRecord(const QString& strQuery);
    QSqlRecord ExecuteRecord(const QString& strQuery, QList<QVariant> lstParameter);
    QSqlRecord ExecuteRecord(const QString& strQuery, QVariant Parameter);

    QSqlQuery ExecuteSqlQuery(const QString& strQuery, QList<QVariant> lstParameter);
    QSqlQuery ExecuteSqlQuery(const QString& strQuery, QVariant Parameter);
    QSqlQuery ExecuteSqlQuery(const QString& strQuery);

    int ExecuteInt(const QString& strQuery);
    int ExecuteInt(const QString& strQuery, QList<QVariant> lstParameter);
    int ExecuteInt(const QString& strQuery, QVariant Parameter);

    bool Execute(const QString& strQuery, QVariant Parameter);
    bool Execute(const QString& strQuery, QList<QVariant> lstParameter);

    QString ExecuteString(const QString& strQuery);

    void ExecuteQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery);
    void ExecuteQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery,
                           QList<QVariant> lstParameter);
    void ExecuteQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery, QVariant Parameter);

private:
    QSqlDatabase m_db;  //连接
    QString m_strDatabase;  //数据库名称
};

class SqliteUtilFactory
{
public:
    static SqliteUtil * instance(QObject *parent = nullptr, QString strDatabase = "my.db")
    {
        return new SqliteUtil(parent,strDatabase);
    }
};

#endif // SQLITEUTIL_H
