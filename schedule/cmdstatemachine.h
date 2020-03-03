#ifndef CMDSTATEMACHINE_H
#define CMDSTATEMACHINE_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "mirschedule.h"
#include "requestqueuecon.h"
#include "mircmdcon.h"
#include "itemdef.h"


class CmdStateMachine : public QObject
{
    Q_OBJECT
public:
    explicit CmdStateMachine(QObject *parent = nullptr,
                             MIRSchedule * mirSchedule = nullptr);
    ~CmdStateMachine();

    //依赖 基本数据的来源,实体类
    MIRSchedule * mirSchedule = nullptr;

    //请求队列处理,立即返回结果
    RequestQueueCON * con1 = nullptr;
    //MIR命令处理 (需延时返回结果)
    MIRCmdCON * con2 = nullptr;
    MIRCmdCON * con3 = nullptr;
    MIRCmdCON * con4 = nullptr;
    MIRCmdCON * con5 = nullptr;


    //成员
    QTimer * timer = nullptr;
    static int cmdState;    //锁
    bool IsBusy();
    void SetBusy();
    bool IsMissionEmpty();
    bool IsPositionEmpty();
    bool IsRequestInQueue(QString bingqu);

    int  appState = 0;      //协程program counter
    void InsertRequest(QString bingqu, QString requestContent); //协程
    void DeleteRequest(QString bingqu); //协程
    void ProcessRequest();              //协程
    void UpdateRequest();               //协程

    void FixRequest();                  //修复已经在任务列表中的请求（未知原因没有正确执行）
    void ChargeRequest();               //判断充电的条件，并在没有其他任务的情况下，下发任务

    //position列表需要时序处理 先position 再name x y
    void GetAllPosition();
    void GetMission();

    //go命令和开箱命令 立即返回 无需协程
    void GoCmdRequest();
    void OpenCmdRequest();


    //协程处理的临时成员(也就是缓存，既然是缓存，就要防止被中途意外修改！)
    QString bingqu; //未执行的请求
    QString dest;   //未执行的目的地
signals:
    void CmdSMFinished(int type, bool result, const QString detial);
public slots:
};

class CmdStateMachineFactory
{
public:
    static CmdStateMachine * instance(QObject *parent = nullptr,
                                      MIRSchedule * mirSchedule = nullptr)
    {
        return new CmdStateMachine(parent,mirSchedule);;
    }
};

#endif // CMDSTATEMACHINE_H
