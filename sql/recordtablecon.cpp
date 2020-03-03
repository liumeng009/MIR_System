#include "recordtablecon.h"
#include <QDateTime>
#include <QDebug>


RecordTableCON * RecordTableCONFactory::m_pInstance = nullptr;

RecordTableCON::RecordTableCON(QObject *parent) : QObject(parent)
{
    database = "my.db";
    table = "记录表";

    createConnection(database);
    createTable("CREATE TABLE 记录表 (\
                id      INTEGER PRIMARY KEY AUTOINCREMENT,\
                date    DATE    NOT NULL,\
                boxid   INTEGER NOT NULL,\
                user    STRING  NOT NULL,\
                type    STRING  NOT NULL,\
                content STRING  NOT NULL)");
}

bool RecordTableCON::createConnection(const QString &database)
{
    //与数据库建立连接
    if (QSqlDatabase::contains("my_conn"))
    {
        m_db = QSqlDatabase::database("my_conn");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "my_conn");
    }
    //设置数据库名
    m_db.setDatabaseName(database);
    //打开数据库
    if (!m_db.open())
    {
        qDebug() << "Open database failed!";
        return false;
    }
    else
    {
        //qDebug()<<QString::number((unsigned long)&(this->m_db));
        return true;
    }
}
bool RecordTableCON::createTable(const QString &strQuery)
{
    bool success = false;

    QSqlQuery query(m_db);
    //query.prepare("CREATE TABLE people(id INTEGER PRIMARY KEY, name TEXT);");
    query.prepare(strQuery);

    if (!query.exec())
    {
        qDebug() << strQuery;
        qDebug() << "Couldn't create the table : one might already exist.";
        success = false;
    }

    return success;
}
int RecordTableCON::GetRecordNum()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare("SELECT id FROM 记录表");
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }
    return nRecordCounts;
}
int RecordTableCON::InsertRecord( int boxNum,
                                  int type,
                                  QString user,
                                  QString content)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString strType = (type == 1? QStringLiteral("取货"):QStringLiteral("补货"));
    QString str = QString("INSERT INTO 记录表 VALUES (NULL,'%1',%2,'%3','%4','%5')").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(boxNum).arg(user).arg(strType).arg(content);
    qDebug()<<str;
    query.prepare(str);

    if(query.exec())
        return query.lastInsertId().toInt();    //成功
    else
        return 0;                               //失败 正常从1开始
}
QVariant RecordTableCON::GetRecord(int id)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 记录表 WHERE id = %1").arg(id);
    qDebug()<<str;
    query.prepare(str);
    query.exec();

    QString name;
    while(query.next())
    {
        name = query.value(1).toString();
    }
    return name;
}
QList<QVariant> RecordTableCON::GetRecord(QString startDate, QString endDate)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT * FROM 记录表 WHERE date > %1 and date < %2").arg(startDate,endDate);
    qDebug()<<str;
    query.prepare(str);
    if(query.exec())
    {
        //从结果集 提取 string
    }
    else
    {
        //返回空的值
    }
}
QVector<QStringList> RecordTableCON::GetRecordResult(QString startDate,
                                                     QString endDate,
                                                     int boxNum,
                                                     QString type,
                                                     QString user)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT * FROM 记录表 WHERE date > '%1' and date < '%2' and boxid = %3 and type = '%4' and user = '%5'").arg(startDate).arg(endDate).arg(boxNum).arg(type).arg(user);
    qDebug()<<str;
    query.prepare(str);
    if(query.exec())
    {
        //从结果集 提取 string
        QVector<QStringList> result;
        while(query.next())
        {
            QStringList strList;
            strList << query.value(0).toString() \
                    << query.value(1).toString() \
                    << query.value(2).toString() \
                    << query.value(3).toString() \
                    << query.value(4).toString() \
                    << query.value(5).toString();
            result.append(strList);
        }
        return result;
    }
    else
    {
        //返回空的值
        return QVector<QStringList>();
    }
}

/*
 * 按照开始时间，结束时间，箱体（有限个数）
 * 检索出记录表中的所有记录
 *
*/
QVector<QStringList> RecordTableCON::GetRecordResult(QString startDate,
                                                     QString endDate,
                                                     int boxNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT * FROM 记录表 WHERE date > '%1' and date < '%2' and boxid = %3 ").arg(startDate).arg(endDate).arg(boxNum);
    qDebug()<<str;
    query.prepare(str);
    if(query.exec())
    {
        //从结果集 提取 string
        QVector<QStringList> result;
        while(query.next())
        {
            QStringList strList;
            strList << query.value(0).toString() \
                    << query.value(1).toString() \
                    << query.value(2).toString() \
                    << query.value(3).toString() \
                    << query.value(4).toString() \
                    << query.value(5).toString();
            result.append(strList);
        }
        return result;
    }
    else
    {
        //返回空的值
        return QVector<QStringList>();
    }
}
