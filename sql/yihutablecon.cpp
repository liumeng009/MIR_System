#include "yihutablecon.h"
#include <QTextCodec>
#include <QByteArray>

YihuTableCON * YihuTableCONFactory::m_pInstance = nullptr;

YihuTableCON::YihuTableCON(QObject *parent) : QObject(parent)
{
    database = "my.db";
    table = "医护表";

    createConnection(database);
    createTable("CREATE TABLE 医护表 ( \
                id       INT (2)      PRIMARY KEY \
                                      UNIQUE \
                                      NOT NULL, \
                name     STRING (4)   NOT NULL, \
                bingquid INT (1)      NOT NULL, \
                finger   STRING (500))");
}

bool YihuTableCON::createConnection(const QString &database)
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
bool YihuTableCON::createTable(const QString &strQuery)
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

int YihuTableCON::GetYihuNum()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare("SELECT id FROM 医护表");
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }
    return nRecordCounts;
}
bool YihuTableCON::IsYinuExist(int yihuNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id FROM 医护表 WHERE id= %1").arg(yihuNum);
    query.prepare(str);
    query.exec();

    int nRecordCounts = 0;
    while(query.next())
    {
        nRecordCounts += 1;
    }

    if(nRecordCounts == 0)
        return false;
    else
        return true;
}
bool YihuTableCON::IsFingerExist(int yihuNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT finger FROM 医护表 WHERE id= %1").arg(yihuNum);
    query.prepare(str);
    query.exec();

    QString finger;
    while(query.next())
    {
        finger = query.value(0).toString();
        if(finger.isEmpty())
            return false;
        else
            return true;
    }

    return false;
}
QString YihuTableCON::GetYihuName(int yihuNum)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    QSqlQuery query(m_db);
    QString str = QString("SELECT id,name FROM 医护表 WHERE id= %1").arg(yihuNum);
    query.prepare(str);
    query.exec();

    QString name;
    while(query.next())
    {
        name = query.value(1).toString();
    }
    return name;
}
bool YihuTableCON::SaveYihuFinger(int yihuNum, QString finger)
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    //判断是否存在
    if(!IsYinuExist(yihuNum))
        return false;


    QSqlQuery query(m_db);
    QString str = QString("UPDATE 医护表 SET finger = '%1' WHERE id = %2").arg(finger).arg(yihuNum);
    qDebug()<<str;
    query.prepare(str);
    return query.exec();
}
QVector<YihuData> YihuTableCON::GetYihuResultWithFinger()
{
    if(!m_db.isOpen())
    {
        createConnection(database);
    }
    /*转换为数组所需*/
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QString abc;
    QByteArray array;

    QSqlQuery query(m_db);
    QString str = QString("SELECT * FROM 医护表 WHERE finger is not NULL");
    qDebug()<<str;
    query.prepare(str);

    QVector<YihuData> result;
    if(query.exec())
    {
        while(query.next())
        {
            YihuData data;
            data.id = query.value(0).toInt();
            data.strName = query.value(1).toString();

            /*转化为数组开始*/
            abc = data.strName;
            abc = gbk->toUnicode(abc.toLocal8Bit());    //保存GBK形式

            array = abc.toLocal8Bit();
            if(array.size() > 8)
                array.resize(8);

            abc = gbk->toUnicode(array);    //保存GBK形式

            if(abc.toLocal8Bit().size() > 8)
                abc.clear();

            memset(data.name,0,8);
            memcpy(data.name,
                   abc.toLocal8Bit().data(),
                   abc.toLocal8Bit().size());
            /*转化为数组结束*/

            data.bingquId = query.value(2).toInt();
            data.strFinger = query.value(3).toString();
            qDebug()<<data.strFinger;
            result.append(data);
        }
        return result;
    }
    else
    {
        //返回空的值
        return QVector<YihuData>();
    }
}
