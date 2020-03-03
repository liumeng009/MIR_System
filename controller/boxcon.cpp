#include "boxcon.h"
#include "itemdef.h"
#include "rmsfactory.h"
#include "faderwidget.h"

/* serveworker++++++>
 *                BoxCON++++++>
 *                              rms
 *                BoxCON<-----
 * serveworker<------
*/
BoxCON::BoxCON(QObject *parent) : QObject(parent)
  ,rms(RMSFactory())
  ,bingquTableCON(BingquTableCONFactory::instance(this))
  ,releaseTimer(new QTimer(this))
{
    releaseTimer->start(5000);
    connect(releaseTimer,SIGNAL(timeout()),
          this,SLOT(TimerFunc()));
}
BoxCON::~BoxCON()
{

}
void BoxCON::RequestInsert(int type,QString bingqu, QString requestContent)
{
    ApplyUnit * applyUnitClient = ApplyUnitFactory::instance(this);
    ApplyJson * applyJsonClient = ApplyJsonFactory::instance(this);
    ApplyUnit * applyUnitServer = ApplyUnitFactory::instance(this);
    ApplyJson * applyJsonServer = ApplyJsonFactory::instance(this);

    //首先判断bingqu是否存在
    if(bingquTableCON->IsBingquExist(bingqu))
    {
        int boxid = 0;
        /*获取DB中病区的id*/
        boxid = bingquTableCON->GetBingquBoxId(bingqu);
        /*判断内存中id是否存在*/
        if(rms->IsBoxIdExist(boxid))
        {
            //判断client请求是否可以被server满足
            if(type == 1)
            {
                //材料室请求，不需要验证了
            }
            else if(type == 2)
            {
                /*实际请求*/
                applyJsonClient->QstringToJson(requestContent);
                applyUnitClient->FromJson(applyJsonClient->ApplyJsonMessage);

                /*当前可用stock,新的时间点（提交时间点）*/
                applyJsonServer->QstringToJson(RMSFactory()->GetBoxUnit(boxid)->GetAbleStock());
                applyUnitServer->FromJson(applyJsonServer->ApplyJsonMessage);

                if(applyUnitClient->LessThan(applyUnitServer))
                {
                    //可以继续申请
                    //接下去继续执行
                }
                else
                {
                    //次订单不可以申请
                    emit CmdBoxFinished(BOX_REQUEST_INSERT,false,QVariant(), QStringLiteral("库存已不足，被其他病区申请"));
                    return;
                }
            }
            else
            {
                emit CmdBoxFinished(BOX_REQUEST_INSERT,false,QVariant(), QStringLiteral("请求ID无法识别"));
            }


            //boxid存在之后，就要开始操作
            /*经典三步
             * 实例化
             * 连接
             * 调用
             * 释放（有父子对象统一释放）
            */
            cmdStateMachine = CmdStateMachineFactory::instance(this,rms->GetBoxUnit(boxid)->mirSchedule);
            connect(cmdStateMachine,SIGNAL(CmdSMFinished(int,bool,QString)),
                    this,SLOT(CmdSMFinished(int,bool,QString)));
            cmdStateMachine->InsertRequest(bingqu,requestContent);
            /*连接之后，等待信号回复*/
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_INSERT,false,QVariant(), QStringLiteral("no boxid"));
        }
    }
    else
    {
        emit CmdBoxFinished(BOX_REQUEST_INSERT,false, QVariant(), QStringLiteral("no bingqu"));
    }
}
void BoxCON::RequestDelete(QString bingqu)
{
    //bingqu ----> boxid ：由bingqu得出boxid的全过程
    if(bingquTableCON->IsBingquExist(bingqu))
    {
        int boxid = 0;
        boxid = bingquTableCON->GetBingquBoxId(bingqu);
        if(rms->IsBoxIdExist(boxid))
        {
            //boxid存在之后，就要开始操作
            /*经典三步
             * 实例化
             * 连接
             * 调用
             * 释放（有父子对象统一释放）
            */
            cmdStateMachine = CmdStateMachineFactory::instance(this,rms->GetBoxUnit(boxid)->mirSchedule);
            connect(cmdStateMachine,SIGNAL(CmdSMFinished(int,bool,QString)),
                    this,SLOT(CmdSMFinished(int,bool,QString)));
            cmdStateMachine->DeleteRequest(bingqu);
            /*连接之后，等待信号回复*/
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_DELETE,false,QVariant(), QStringLiteral("no boxid"));
        }
    }
    else
    {
        emit CmdBoxFinished(BOX_REQUEST_DELETE,false, QVariant(), QStringLiteral("no bingqu"));
    }
}
void BoxCON::RequestStatusQuery(QString bingqu)
{
    //bingqu ----> boxid ：由bingqu得出boxid的全过程
    if(bingquTableCON->IsBingquExist(bingqu))
    {
        int boxid = 0;
        boxid = bingquTableCON->GetBingquBoxId(bingqu);
        if(rms->IsBoxIdExist(boxid))
        {
            /*申请对象*/
            cmdStateMachine = CmdStateMachineFactory::instance(this,rms->GetBoxUnit(boxid)->mirSchedule);
            /*立刻返回数据，就不需要信号与槽了*/
//            connect(cmdStateMachine,SIGNAL(CmdSMFinished(int,bool,QString)),
//                    this,SLOT(CmdSMFinished(int,bool,QString)));

            //计算可申请库存。。。
            STATE_INFO data = rms->GetBoxUnit(boxid)->mirSchedule->state;
            StateData backData;
            backData.MIRState = data.MIRState;
            backData.MIRTargetBingqu = data.MIRTargetBingqu;
            backData.missionState = data.missionState;
            backData.missionText = data.missionText;
            backData.stateId = data.stateId;
            backData.stateText = data.stateText;
            backData.requestExist = cmdStateMachine->IsRequestInQueue(bingqu);

            //上句 用完释放
            cmdStateMachine->deleteLater();

            //emit返回状态
            emit CmdBoxFinished(BOX_REQUEST_STATUS_QUERY,true,QVariant::fromValue(backData), QStringLiteral("state"));
            return;
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_STATUS_QUERY,false,QVariant(), QStringLiteral("no boxid"));
            return;
        }
    }
    else
    {
        emit CmdBoxFinished(BOX_REQUEST_STATUS_QUERY,false, QVariant(), QStringLiteral("no bingqu"));
        return;
    }
}
void BoxCON::RequestStockQuery(QString bingqu)
{
    //bingqu ----> boxid
    if(bingquTableCON->IsBingquExist(bingqu))
    {
        int boxid = -1;
        boxid = bingquTableCON->GetBingquBoxId(bingqu);
        if(rms->IsBoxIdExist(boxid))
        {
            //计算可申请库存。。。
            QString ableString = rms->GetBoxUnit(boxid)->GetAbleStock();
            if(ableString.isEmpty())
            {
                emit CmdBoxFinished(BOX_REQUEST_STOCK_QUERY,false,QVariant(), QStringLiteral("calulate error"));
            }
            else
            {
                emit CmdBoxFinished(BOX_REQUEST_STOCK_QUERY,true,QVariant::fromValue(ableString), QStringLiteral("get able stock"));
            }
            return;
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_STOCK_QUERY,false,QVariant(),QStringLiteral("no boxid"));
            return;
        }
    }
    else
    {
        emit CmdBoxFinished(BOX_REQUEST_STOCK_QUERY,false,QVariant(),QStringLiteral("no bingqu:")+ bingqu);
        return;
    }
}

/*
 * 槽函数专门处理sm状态机的返回信息
 * sm状态机返回的数据就是 <类型，成功/失败，描述>
 * 之所以有这个槽函数，是因为sm状态机返回需要时间，不能阻塞,才使用的
 * 平板请求的所有操作，均是由sm状态机协程完成，只一步就得到结果
 * 注：对于非延时的操作，可以不用在槽函数处理!直接处理就行！
*/
void BoxCON::CmdSMFinished(int type, bool result, const QString detial)
{
    switch (type) {
    case SM_DELETE_REQUEST:
        if(result)
        {
            emit CmdBoxFinished(BOX_REQUEST_DELETE,true,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","删除请求成功"+detial);
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_DELETE,false,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","删除请求失败"+detial);
        }
        break;
    case SM_INSERT_REQUEST:
        if(result)
        {
            emit CmdBoxFinished(BOX_REQUEST_INSERT,true,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","插入请求成功"+detial);
        }
        else
        {
            emit CmdBoxFinished(BOX_REQUEST_INSERT,false,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","插入请求失败"+detial);
        }
        break;
    case SM_GET_MISSION:
        if(result)
        {
            emit CmdBoxFinished(SM_GET_MISSION,true,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","获取任务成功"+detial);
        }
        else
        {
            emit CmdBoxFinished(SM_GET_MISSION,false,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","获取任务失败"+detial);
        }
        break;
    case SM_CHARGE_REQUEST:
        if(result)
        {
            emit CmdBoxFinished(SM_CHARGE_REQUEST,true,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","充电成功"+detial);
        }
        else
        {
            emit CmdBoxFinished(SM_CHARGE_REQUEST,false,
                                QVariant(), detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","充电失败"+detial);
        }
        break;
    default:
        break;
    }
}

void BoxCON::TimerFunc()
{
    emit CmdBoxFinished(BOX_REQUEST_TIMEOUT,false,
                        QVariant(), QStringLiteral("box cmd time out"));

    //不delete，由上层调用者析构
}
