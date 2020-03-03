#ifndef BOXTABLECON_H
#define BOXTABLECON_H

#include <QObject>
#include "sqliteutil.h"

class BoxTableCON : public QObject
{
    Q_OBJECT
public:
    explicit BoxTableCON(QObject *parent = nullptr);

    //成员
    QString database;
    QString table;
    QSqlDatabase m_db;  //连接
    bool createConnection(const QString& strConn);
    bool createTable(const QString &strQuery);

    int GetBoxNum();
    bool IsBoxExist(int boxNum);
    QString GetStock(int boxNum);
    bool SaveStock(int boxNum,QString stock);
    QSqlQuery GetBoxRecord();
    bool IsStockEmpty(int boxNum);

signals:

public slots:
};

/*针对一个表的单例工厂*/
class BoxTableCONFactory
{
public:
    static BoxTableCON * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new BoxTableCON(parent);
        }
        return m_pInstance;
    }

    static void Release()
    {
        if (m_pInstance != NULL)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }
public:
    static BoxTableCON * m_pInstance;
};

#endif // BOXTABLECON_H
