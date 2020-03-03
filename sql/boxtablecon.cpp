#include "boxtablecon.h"

BoxTableCON * BoxTableCONFactory::m_pInstance = nullptr;

BoxTableCON::BoxTableCON(QObject *parent) : QObject(parent)
{
    database = "my.db";
    table = "箱体配置表";

    createConnection(database);
    createTable("CREATE TABLE 箱体配置表 (      \
                id    INT (2)     PRIMARY KEY \
                                  UNIQUE      \
                                  NOT NULL,   \
                ip    STRING (20),            \
                port  STRING (5),             \
                stock STRING                  \
            )");
}

bool BoxTableCON::createConnection(const QString &database)
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
bool BoxTableCON::createTable(const QString &strQuery)
{
    bool success = false;

    QSqlQuery query(m_db);
    query.prepare(strQuery);

    if (!query.exec())
    {
        qDebug() << strQuery;
        qDebug() << "Couldn't create the table : one might already exist.";
        success = false;
    }

    return success;
}
int BoxTableCON::GetBoxNum()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare("SELECT id FROM 箱体配置表");
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }
    return nRecordCounts;
}
bool BoxTableCON::IsBoxExist(int boxNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    QString str = QString("SELECT id FROM 箱体配置表 WHERE id = %1").arg(boxNum);
    qDebug()<<str;
    query.prepare(str);
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }

    if(nRecordCounts > 0)
        return true;
    else
        return false;
}
QString BoxTableCON::GetStock(int boxNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,stock FROM 箱体配置表 WHERE id = %1").arg(boxNum);
    query.prepare(str);
    query.exec();

    QString stock;
    while(query.next())
    {
        stock = query.value(1).toString();
    }
    return stock;
}
bool BoxTableCON::IsStockEmpty(int boxNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,stock FROM 箱体配置表 WHERE id = %1").arg(boxNum);
    query.prepare(str);
    query.exec();

    QString stock;
    while(query.next())
    {
        stock = query.value(1).toString();
    }

    return stock.isEmpty();
}
bool BoxTableCON::SaveStock(int boxNum,QString stock)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("UPDATE 箱体配置表 SET stock = '%1' WHERE id = %2").arg(stock).arg(boxNum);
    query.prepare(str);
    return query.exec();
}
QSqlQuery BoxTableCON::GetBoxRecord()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM 箱体配置表");
    query.exec();

    return query;
}
