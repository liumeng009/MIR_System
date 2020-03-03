#ifndef BINGQUTABLECON_H
#define BINGQUTABLECON_H

#include <QObject>
#include "sqliteutil.h"

/* 病区表数据库自定义操作的类：
 * 新建一个连接、操作bingquTable、及时返回结果
 * （系统中只有单例对象）
*/

class BingquTableCON : public QObject
{
    Q_OBJECT
public:
    explicit BingquTableCON(QObject *parent = nullptr);

    //成员
    QString database;
    QString table;
    QSqlDatabase m_db;  //连接
    bool createConnection(const QString& strConn);
    bool createTable(const QString &strQuery);

    int     GetBingquNum();
    int     GetBingquId(const QString& bingquName);
    QString GetBingquName(int id);
    int     GetBingquBoxId(QString bingqu);
    bool    IsBingquExist(const QString& bingquName);
    QStringList GetBingqu();
signals:

public slots:
};

/*数据库表(一个表)--->单例工厂*/
class BingquTableCONFactory
{
public:
    static BingquTableCON * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new BingquTableCON(parent);
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
    static BingquTableCON * m_pInstance;
};


#endif // BINGQUTABLECON_H
