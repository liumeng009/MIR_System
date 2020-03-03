#ifndef YIHUTABLECON_H
#define YIHUTABLECON_H

#include <QObject>
#include <QVector>
#include "sqliteutil.h"
#include "itemdef.h"

class YihuTableCON : public QObject
{
    Q_OBJECT
public:
    explicit YihuTableCON(QObject *parent = nullptr);

    //成员
    QString database;
    QString table;
    QSqlDatabase m_db;  //连接
    bool createConnection(const QString& strConn);
    bool createTable(const QString &strQuery);

    bool IsYinuExist(int yihuNum);
    int GetYihuNum();
    QString GetYihuName(int yihuNum);
    bool SaveYihuFinger(int yihuNum, QString finger);
    bool IsFingerExist(int yihuNum);
    QVector<YihuData> GetYihuResultWithFinger();
signals:

public slots:
};

/*针对一个表的单例工厂*/
class YihuTableCONFactory
{
public:
    static YihuTableCON * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new YihuTableCON(parent);
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
    static YihuTableCON * m_pInstance;
};

#endif // YIHUTABLECON_H
