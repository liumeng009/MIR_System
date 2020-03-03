#include "sysconfig.h"


//静态成员重新定义，静态初始化为NULL
SysConfig * SysConfigFactory::m_pInstance = NULL;

SysConfig::SysConfig(QObject *parent) : QObject(parent)
  ,pConfigIni(new QSettings("config.ini", QSettings::IniFormat))
  ,loadFile(new QFile(this))
{
    bingqu = pConfigIni->value("bingqu").toString();
    serverIP = pConfigIni->value("serverIP").toString();
    serverPort = pConfigIni->value("serverPort").toInt();
    taskName = pConfigIni->value("taskName").toString();
    chargeTaskName = pConfigIni->value("chargeTaskName").toString();
    qDebug()<<chargeTaskName;
}
bool SysConfig::LoadJsonConfig(int boxNum)
{
    QString fileName = QString("./appconfig/箱体配置_%1.json").arg(boxNum);
    loadFile->setFileName(fileName);

    //打开文件
    if(!loadFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open projects json";
        return false;
    }

    //获得array
    QByteArray allData = loadFile->readAll();

    //关闭文件
    loadFile->close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!";
        return false;
    }

    //直接写入对象
    configJson = jsonDoc;

    /*一定要返回，否则默认返回false*/
    return true;
}
QJsonDocument SysConfig::GetJsonConfig()
{
   return configJson;
}

SysConfig::~SysConfig()
{
    pConfigIni->deleteLater();
}

