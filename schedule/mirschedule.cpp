#include "mirschedule.h"
#include "httpcon.h"
#include "boxunit.h"

MIRSchedule::MIRSchedule(QObject *parent) : QObject(parent)
  ,boxUnit((BoxUnit *)parent)
{
    state.MIRState = 0;
    state.pos_x = 0.0;
    state.pos_y = 0.0;
    state.battery_percentage = 100;
}
void MIRSchedule::TurnOnFunc()
{
    on = true;
}
void MIRSchedule::TurnOffFunc()
{
    on = false;
}
bool MIRSchedule::GetOnOff()
{
    return on;
}

void MIRSchedule::SetMIR(QString ip, QString port,
                         QString missionName,
                         QString chargeMissionName)
{
    MIRIP = ip;
    MIRPort = port;
    missionSelected = missionName;
    chargeMissionSelected = chargeMissionName;
    URL = "http://"+ MIRIP + "/api/v2.0.0/";
    missionIdSelected  = GetMissionGuid(missionName);  //获得missionGuid
    chargeMissionIdSelected  = GetMissionGuid(chargeMissionName);  //获得missionGuid
}
QString MIRSchedule::GetMissionGuid(QString mission)
{
    foreach (MISSION_INFO item, missionGuidList) {
        if(mission == item.name)
            return item.guid;
    }
    return "";
}

void MIRSchedule::SetMissionGuid()
{
    /*普通任务*/
    missionIdSelected = GetMissionGuid(missionSelected);

    /*充电任务*/
    chargeMissionIdSelected = GetMissionGuid(chargeMissionSelected);
    qDebug()<<chargeMissionIdSelected<<chargeMissionSelected;
}

QStringList MIRSchedule::GetRequestContent()
{
    QStringList requestContentList;
    foreach (RequestNode * item, requestList) {
        //未执行完，并且执行中的没有取货, 而且必须是手术室请求
        if(item->state < 2 && item->opened == 0 && item->type == 1)
        {
            requestContentList.append(item->requestContent);
        }
    }
    return requestContentList;
}
void MIRSchedule::ResetTheMirSchedule()
{
    //动作很危险！ 还需要更详细的处理
    requestList.clear();
}
double MIRSchedule::GetBattery()
{
    return state.battery_percentage;
}
QString MIRSchedule::GetPositonGuid(QString dest)
{
    foreach (POSITION_INFO item, positonGuidList) {
        if(item.name == dest)
            return item.guid;
    }
    return "";
}
double MIRSchedule::GetPositonX(QString dest)
{
    foreach (POSITION_INFO item, positonGuidList) {
        if(item.name == dest)
            return item.X;
    }
    return 0.0;
}
double MIRSchedule::GetPositonY(QString dest)
{
    foreach (POSITION_INFO item, positonGuidList) {
        if(item.name == dest)
            return item.Y;
    }
    return 0.0;
}

