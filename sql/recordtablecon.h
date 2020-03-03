#ifndef RECORDTABLECON_H
#define RECORDTABLECON_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class RecordTableCON : public QObject
{
    Q_OBJECT
public:
    explicit RecordTableCON(QObject *parent = nullptr);

    //成员
    QString database;
    QString table;
    QSqlDatabase m_db;  //连接
    bool createConnection(const QString& strConn);
    bool createTable(const QString &strQuery);

    int GetRecordNum();
    int InsertRecord( int boxNum,
                      int type,
                      QString user,
                      QString content);
    QVariant GetRecord(int id);
    QList<QVariant> GetRecord(QString startDate, QString endDate);
    QVector<QStringList> GetRecordResult(QString startDate,
                                         QString endDate,
                                         int boxNum,
                                         QString type,
                                         QString user);
    QVector<QStringList> GetRecordResult(QString startDate,
                                         QString endDate,
                                         int boxNum);
signals:

public slots:
};

/*针对一个表的单例工厂*/
class RecordTableCONFactory
{
public:
    static RecordTableCON * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new RecordTableCON(parent);
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
    static RecordTableCON * m_pInstance;
};


#endif // RECORDTABLECON_H
