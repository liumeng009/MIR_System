#ifndef SYSCONFIG_H
#define SYSCONFIG_H

#include <QObject>
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>
#include <QByteArray>
#include <QIODevice>

class SysConfig : public QObject
{
    Q_OBJECT
public:
    explicit SysConfig(QObject *parent = nullptr);
    ~SysConfig();

    /*配置文件*/
    QSettings * pConfigIni = nullptr;
    QFile * loadFile;

    QJsonDocument configJson;
    QString bingqu;
    QString serverIP;
    int serverPort;
    QString taskName;   //任务名称
    QString chargeTaskName;   //任务名称

    bool LoadJsonConfig(int boxNum);
    QJsonDocument GetJsonConfig();
};

class SysConfigFactory
{
public:
    static SysConfig * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new SysConfig(parent);
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
    static SysConfig * m_pInstance;
};

#endif // SYSCONFIG_H
