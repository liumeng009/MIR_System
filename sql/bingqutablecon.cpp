#include "bingqutablecon.h"

BingquTableCON * BingquTableCONFactory::m_pInstance = nullptr;

BingquTableCON::BingquTableCON(QObject *parent) : QObject(parent)
{
    database = QStringLiteral("my.db");
    table = QStringLiteral("病区表");

    createConnection(database);
    createTable("CREATE TABLE 病区表 (           \
                id    INT (1)  PRIMARY KEY      \
                               NOT NULL         \
                               UNIQUE,          \
                name  TEXT (8) NOT NULL,        \
                boxid INTEGER  NOT NULL         \
            )");
}

bool BingquTableCON::createConnection(const QString &database)
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
bool BingquTableCON::createTable(const QString &strQuery)
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
int BingquTableCON::GetBingquNum()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare("SELECT * \
                  FROM 病区表");
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }
    return nRecordCounts;
}
int BingquTableCON::GetBingquId(const QString& bingquName)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 病区表 WHERE name= '%1'").arg(bingquName);
    query.prepare(str);
    query.exec();

    int id = 0;
    while(query.next())
    {
        id = query.value(0).toInt();
    }
    return id;
}
QString BingquTableCON::GetBingquName(int id)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 病区表 WHERE id = %1").arg(id);
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
int BingquTableCON::GetBingquBoxId(QString bingqu)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT boxid FROM 病区表 WHERE name = '%1'").arg(bingqu);
    query.prepare(str);
    query.exec();

    int boxId = -1;
    while(query.next())
    {
        boxId = query.value(0).toInt();
    }
    return boxId;
}
bool BingquTableCON::IsBingquExist(const QString& bingquName)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 病区表 WHERE name= '%1'").arg(bingquName);
    qDebug()<<str;
    query.prepare(str);
    query.exec();

    int id = 0;
    while(query.next())
    {
        id = query.value(0).toInt();
        return true;
    }
    return false;
}
QStringList BingquTableCON::GetBingqu()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 病区表");
    qDebug()<<str;
    query.prepare(str);
    query.exec();

    QStringList strlist;
    while(query.next())
    {
        strlist << query.value(1).toString();
    }
    return strlist;
}

