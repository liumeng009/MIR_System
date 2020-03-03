#ifndef MIRSCHEDULE_H
#define MIRSCHEDULE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include "table.h"
#include "requestnode.h"
#include "sql/boxtablecon.h"

class BoxUnit;


/*
 * 每一个BoxUnit --- MIRSChedule （1对1关系）
 *
*/

class MIRSchedule : public QObject
{
    Q_OBJECT
public:
    explicit MIRSchedule(QObject *parent = nullptr);

    //单例依赖
    BoxUnit * boxUnit = nullptr;    //既是父对象，也是一对一关系

    //成员
    bool on = true;     //调度系统的开关，目前没有用到
    QString MIRIP;      //数据库刷新，http使用
    QString MIRPort;    //数据库刷新，http使用
    QString URL;        //对于资源的描述。。。http://10.10.10.13
    STATE_INFO             state;   //关于MIR机器人的信息 静态的和动态的
    QList <MISSION_INFO>   missionGuidList;     //mission列表 <K-V> GUID和name
    QString                missionSelected;     //mission name <"mission_test">
    QString                missionIdSelected;   //mission guid <"1234-5678-1234-5678">
    QString                chargeMissionSelected;     //charge mission name <"mission_test2">
    QString                chargeMissionIdSelected;   //charge mission guid <"1234-5678-1234-5678">


    QList <POSITION_INFO>  positonGuidList;    //位置地图列表，查找地名，从中取出guid <K-V>

    QList <RequestNode *>  requestList; //请求列表 调度的驱动执行链！调度程序主要解释它。

    int cmdQueueState = 0;  //命令占用或非占用(目前没有必要使用，因为所有都是单线程，同步执行的
                            //同步就是严格按照顺序，不会出现书序错乱的情况)

    //开关功能
    void TurnOnFunc();
    void TurnOffFunc();
    bool GetOnOff();

    void SetMIR(QString ip, QString port,
                QString missionName,
                QString chargeMissionName);
    QString GetPositonGuid(QString dest);
    double GetPositonX(QString dest);
    double GetPositonY(QString dest);
    QString GetMissionGuid(QString mission);
    void SetMissionGuid();
    QStringList GetRequestContent();
    void ResetTheMirSchedule();
    double GetBattery();

signals:

public slots:


};

class MIRScheduleFactory
{
public:
    static MIRSchedule * instance(QObject *parent = nullptr)
    {
        return new MIRSchedule(parent);;
    }
};

#endif // MIRSCHEDULE_H
