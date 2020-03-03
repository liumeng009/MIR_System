#include "cmdstatemachine.h"
#include "sql/boxtablecon.h"
#include "sql/recordtablecon.h"
#include "applyform/applyform.h"
#include "applyform/applyjson.h"
#include "boxunit.h"

int CmdStateMachine::cmdState = 0;

CmdStateMachine::CmdStateMachine(QObject *parent, MIRSchedule *mirSchedule) : QObject(parent)
  ,con1(RequestQueueCONFactory::instance(this,mirSchedule))
  ,con2(MIRCmdCONFactory::instance(this,mirSchedule))
  ,con3(MIRCmdCONFactory::instance(this,mirSchedule))
  ,con4(MIRCmdCONFactory::instance(this,mirSchedule))
  ,con5(MIRCmdCONFactory::instance(this,mirSchedule))
  ,timer(new QTimer(this))
{
    this->mirSchedule = mirSchedule;
    timer->start(10000);

    connect(timer, &QTimer::timeout,
            this,[this](){
        this->cmdState = 0;
        emit CmdSMFinished(SM_TIME_OUT ,false, QStringLiteral("cmd sm 5s time out"));
    });

//    connect(this, &CmdStateMachine::CmdSMFinished,
//            this,[=](){this->cmdState = 0;});
}
CmdStateMachine::~CmdStateMachine()
{
    timer->stop();
    timer->disconnect();
    con1->disconnect();
    con2->disconnect();
    con3->disconnect();
    disconnect();
}
/*如果busy说明其他对象正在占用，操作可能引起队列混乱*/
bool CmdStateMachine::IsBusy()
{
    if(cmdState == 1)
        return true;
    else
        return false;
}
void CmdStateMachine::SetBusy()
{
    cmdState = 1;
}
bool CmdStateMachine::IsMissionEmpty()
{
    if(mirSchedule->missionGuidList.size() == 0)
        return true;
    else
        return false;
}
bool CmdStateMachine::IsPositionEmpty()
{
    if(mirSchedule->positonGuidList.size() == 0)
        return true;
    else
    {
        foreach (POSITION_INFO item, mirSchedule->positonGuidList) {
            if(item.name.isEmpty())
                return true;    //出现空就是没有完全读取
        }
        return false;
    }
}
bool CmdStateMachine::IsRequestInQueue(QString bingqu)
{
    if(con1->IsRequestExecuting(bingqu) ||
            con1->IsRequestNotExecuting(bingqu))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*
 * 输入：发起者bingqu，请求内容requestContent
 * 输出：完成信号 成功/失败(失败原因)
 * 注意；1.操作不管队列中的未完成项，只针对执行中和未执行的项目
 *      2.分支较多，利用时间和信号两个机制析构自己
 *      3.本身作为父对象，利用半自动垃圾回收
*/
/* 返回type判断
 * GetMission           1
 * PostMissionQueue     2
 * DeleteQueue          3
 * GetStatus            4
 * PostStatus(abort)    5
 * PostStatus(ready)    6
 * GetPosition          7
 * GetPositionXY(name)  8
*/
void CmdStateMachine::InsertRequest(QString bingqu, QString requestContent)
{
    switch (appState) {
    case 0:
        if(!con1->IsQueueEmpty())
        {
            if(con1->IsRequestExecuting(bingqu) ||
                    con1->IsRequestNotExecuting(bingqu))
            {
                //丢弃不插入 此处有信号
                emit CmdSMFinished(SM_INSERT_REQUEST ,false , QStringLiteral("request already exist"));
            }
            else
            {
                if(con1->IsRequestExecuting(QStringLiteral("集散地")) == false)
                {
                    if(con1->InsertRequest(bingqu,requestContent) == true)
                    {
                        //插入成功 此处有信号
                        emit CmdSMFinished(SM_INSERT_REQUEST ,true , QStringLiteral("request insert"));
                    }
                    else
                    {
                        //插入失败 此处有信号
                        emit CmdSMFinished(SM_INSERT_REQUEST ,false , QStringLiteral("request insert error"));
                    }
                }
                else        /*集散地正在执行*/
                {
                    appState = 1;
                    InsertRequest(bingqu,requestContent);   //重新执行
                }
            }
        }
        else
        {
            if(con1->InsertInitialRequest(bingqu,requestContent)) //初始请求
            {
                con1->SetRequestGoCmd(bingqu,1);      //执行！
                emit CmdSMFinished(SM_INSERT_REQUEST ,true , "request insert");
            }
            else
                emit CmdSMFinished(SM_INSERT_REQUEST ,false , "request insert error");
        }
        break;
    case 1:
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 5 && result == true) /*putStatus*/
            {
                appState = 2;
                qDebug()<<detial;
            }
            InsertRequest(bingqu,requestContent);  //递归 在函数体外
        });
        con2->AbortMirExecuting();
        break;                      //结束执行
    case 2:
        connect(con3,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
             qDebug()<<"TYPE"<<type<<"result"<<result;
            if(type == 3 && result == true) /**/
            {
                appState = 3;
                qDebug()<<detial;
                InsertRequest(bingqu,requestContent);  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_INSERT_REQUEST ,false , QStringLiteral("DeleteQueue failed"));
            }
        });
        con3->DeleteQueueFunc();
        break;                       //结束执行
    case 3:
        con1->RequestQueueClear();
        if(con1->InsertInitialRequest(bingqu,requestContent)) //初始请求
        {
            con1->SetRequestGoCmd(bingqu,1);      //执行！
            emit CmdSMFinished(SM_INSERT_REQUEST ,true , QStringLiteral("request insert"));
        }
        else
            emit CmdSMFinished(SM_INSERT_REQUEST ,false , QStringLiteral("request insert error"));
        break;                              //结束执行        
    default:
        break;
    }
}
/*
 * 输入：发起者bingqu
 * 输出：完成信号 成功/失败(失败原因)
 * 注意；1.操作不管队列中的未完成项，只针对执行中和未执行的项目
 *      2.分支较多，利用时间和信号两个机制析构自己
 *      3.本身作为父对象，利用半自动垃圾回收
*/
/* 返回type判断
 * GetMission           1
 * PostMissionQueue     2
 * DeleteQueue          3
 * GetStatus            4
 * PostStatus(abort)    5
 * PostStatus(ready)    6
 * GetPosition          7
 * GetPositionXY(name)  8
*/
void CmdStateMachine::DeleteRequest(QString bingqu)
{
    switch (appState) {
    case 0:
        if(!con1->IsQueueEmpty())
        {
            if(con1->IsRequestExecuting(bingqu) ||
                    con1->IsRequestNotExecuting(bingqu)) //存在这么一个bingqu
            {
                if(con1->IsRequestNotExecuting(bingqu))
                {
                    if(con1->DeleteRequest(bingqu))
                    {
                        emit CmdSMFinished(SM_DELETE_REQUEST ,true , QStringLiteral("request deleted"));
                    }
                    else
                        emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("request delete error"));
                }
                else if(con1->IsRequestExecuting(bingqu))
                {
                    if(con1->IsRequestOpened(bingqu))
                    {
                        //丢弃不插入 此处有信号
                        emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("request can not delete"));
                    }
                    else
                    {
                        appState = 1;
                        DeleteRequest(bingqu);
                    }
                }

            }
            else
            {
                //丢弃不插入 此处有信号
                emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("request not find"));
                return;
            }
        }
        else
        {
            //丢弃不插入 此处有信号
            emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("requestlist nothing"));
            return;
        }
        break;
    case 1:
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 5 && result == true)
            {
                appState = 2;
                qDebug()<<detial;
                DeleteRequest(bingqu);  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("abort failed"));
            }
        });
        con2->AbortMirExecuting();
        break;                      //结束执行
    case 2:
        connect(con3,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 3 && result == true)
            {
                appState = 3;
                qDebug()<<detial;
                DeleteRequest(bingqu);  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_DELETE_REQUEST ,false , "delete queue failed");
            }
        });
        con3->DeleteQueueFunc();
        break;                       //结束执行
    case 3:
        if(con1->DeleteRequestWithGoCmdNext(bingqu))
            emit CmdSMFinished(SM_DELETE_REQUEST ,true , QStringLiteral("request deleted"));
        else
            emit CmdSMFinished(SM_DELETE_REQUEST ,false , QStringLiteral("request deleted failed"));
        break;                              //结束执行
    default:
        break;
    }
}
/*
 * 处理请求，为各个节点的开始提供依据
*/
void CmdStateMachine::ProcessRequest()
{
    switch (appState) {
    case 0:
        if(!con1->IsQueueEmpty())
        {
            bingqu = con1->GetExecutingRequest();   /*获得执行中的bingqu*/
            if(bingqu.isEmpty())    //没有执行中的请求
            {
                bingqu = con1->GetFirstNotExecuteRequest();
                if(bingqu.isEmpty())
                {
                    con1->RequestQueueClear();
                    emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "requestList clear");
                }
                else
                {
                    if(con1->GetRequestGoCmd(bingqu) == 1)
                    {
                        dest = con1->GetNotexecDest(bingqu);
                        appState = 1;
                        ProcessRequest();
                    }
                    else
                    {
                        //信号
                        emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "no action");
                    }
                }
            }
            else                    //有执行中的请求（普通bingqu，集散地，充电站）
            {
                if(bingqu == QStringLiteral("充电站"))
                {
                    /*充电的终止有多个条件！ 要么有（执行中或者未）执行的任务，要么执行完了*/
                    if(con1->HasNotexecNormalRequest())   //如果有其他请求，充电不被允许
                    {
                        //立刻设置自己为完成！ 直接开始处理其他的请求
                        con1->SetRequestState(QStringLiteral("充电站"),2);
                        //设置下一个请求为自动开始
                        con1->SetGoCmdNext(QStringLiteral("充电站"));
                        emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "charging stop complete");
                    }
                    else    /*如果没有未执行的其他的请求*/
                    {
                        if(con1->AllDestComplete(bingqu))
                        {
                            con1->SetRequestState(QStringLiteral("充电站"),2);
                            emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "charging over");
                        }
                        else
                        {
                            emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "no operation");
                        }
                    }
                }
                else    //其他的正常bingqu，包括集散地
                {
                    if(con1->HasExecDest(bingqu))
                    {
                        emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "no operation");
                    }
                    else
                    {
                        dest = con1->GetNotexecDest(bingqu);
                        if(!dest.isEmpty())    //有未执行的目的地
                        {
                            if(con1->GetRequestGoCmd(bingqu) == 1)
                            {
                                appState = 1;
                                ProcessRequest();
                            }
                            else
                            {
                                emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "no operation");
                            }
                        }
                        else        //没有未执行的目的地
                        {
                            //有执行中或者执行完的情况
                            if(con1->AllDestComplete(bingqu))   //全部执行完
                            {
                                //全部执行完之后，就要对次请求进行库存操作
                                //判断请求类型
                                //执行库存操作（类型主要用来判断库存操作的类型）
                                if(con1->GetRequestType(bingqu) == 1)
                                {
                                    //DB获得实际库存
                                    QString stockString = BoxTableCONFactory::instance()->GetStock(mirSchedule->boxUnit->boxNum);
                                    ApplyJson stockJson;
                                    ApplyUnit stockUnit;
                                    ApplyJson clientJson;
                                    ApplyUnit clientUnit;

                                    if(!stockString.isEmpty())
                                    {
                                        stockJson.QstringToJson(stockString);
                                        stockUnit.FromJson(stockJson.ApplyJsonMessage);

                                        if(con1->GetRequestContent(bingqu).isEmpty())
                                        {
                                            //错误 什么也不做
                                            con1->SetRequestState(bingqu,2);
                                            emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "请求内容为空");
                                        }
                                        else
                                        {
                                            /*先获得此次申请的内容JSON*/
                                            clientJson.QstringToJson(con1->GetRequestContent(bingqu));
                                            clientUnit.FromJson(clientJson.ApplyJsonMessage);

                                            /*用库存减去申请*/
                                            if(stockUnit.Minus(&clientUnit))
                                            {
                                                /*如果操作成功，那么库存已经发生变化*/

                                                /*先保存库存*/
                                                stockJson.ApplyJsonMessage = stockUnit.ToJson();

                                                BoxTableCONFactory::instance()->SaveStock(
                                                            mirSchedule->boxUnit->boxNum,
                                                            stockJson.JsonToQstring());

                                                /**将申请单内容写入记录表，注意是clinetJson！**/
                                                RecordTableCONFactory::instance()->InsertRecord(
                                                            mirSchedule->boxUnit->boxNum,
                                                            1, /*1取货 2补货*/
                                                            bingqu,
                                                            clientJson.JsonToQstring());

                                                /*写入之后，设置为已到达*/
                                                con1->SetRequestState(bingqu,2);
                                                emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "库存扣除成功");
                                            }
                                            else
                                            {
                                                //错误，不保存了
                                                con1->SetRequestState(bingqu,2);
                                                emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "库存扣除失败");
                                            }
                                        }
                                    }
                                    con1->SetRequestState(bingqu,2);
                                    emit CmdSMFinished(SM_PROCESS_REQUEST , true , "数据库保存错误");
                                }
                                else    /*如果不是普通病区的申请，就是不用处理表单*/
                                {
                                    con1->SetRequestState(bingqu,2);
                                    emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "one request complete");
                                }

                            }
                            else    //有执行中的情况
                            {
                                emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "one request not complete");

                            }
                            //信号
                        }
                    }
                }
            }
        }
        else
        {
            //队列中无请求 此处有信号
            emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "no request");
            return;
        }
        break;
    case 1: /*清空队列中因为异常没有完成的任务*/
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 3 && result == true)
            {
                appState = 2;
                ProcessRequest();  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "clear queue failed" + detial);
            }
        });
        con2->DeleteQueueFunc();
        break;                      //结束执行
    case 2:
        connect(con3,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 2 && result == true)
            {
                //前提：bingqu都已确认
                //只有执行任务成功了，才可以清除gocmd和设置requestState
                con1->SetRequestGoCmd(bingqu,0);

                /*具体执行的肯定是某一个dest ，所以一定要设置*/
                con1->SetRequestState(bingqu,1);
                con1->SetDestState(bingqu,dest,1);

                appState = 3;
                ProcessRequest();  //递归 在函数体外

//                if(con1->GetRequestType(bingqu) != 4)
//                {
                    //为什么要在这里排除充电的情况，也就是充电不执行，TMD奇怪！！！
//                }
            }
            else
            {
                emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "execTask failed");
            }
        });
        con3->ExecTask(dest);
        qDebug()<<"RequestProcess(): execTask "+dest;
        break;                      //执行
    case 3:
        connect(con4,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 5 && result == true)
            {
                /*此举指示保证，一定要让MIR运行*/
                appState = 4;
                ProcessRequest();  //递归 在函数体外
                qDebug()<<"StartMirExecuting();";
            }
            else
            {
                emit CmdSMFinished(SM_PROCESS_REQUEST ,false , "ready failed");
            }
        });
        con4->StartMirExecuting();
        break;
    case 4:
        //更新mirSchedule的状态
        mirSchedule->state.MIRTargetBingqu = dest;
        mirSchedule->state.MIRState = 1;
        //真正的队列执行
        con1->SetDestState(bingqu,dest,1);
        emit CmdSMFinished(SM_PROCESS_REQUEST ,true , "one dest started");
        break;
    default:
        break;
    }

}
/*
 * 更新状态，为各个节点的结束提供依据
*/
void CmdStateMachine::UpdateRequest()
{
    switch (appState) {
    case 0:
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 4 && result == true)
            {
                appState = 1;
                this->UpdateRequest();  //递归 在函数体外
            }
            else
            {
                emit this->CmdSMFinished(SM_UPDATE_REQUEST ,false , QStringLiteral("update Request failed"));
            }
        });
        con2->GetMirState();
        break;                      //结束执行
    case 1:
        //此时数据已经更新到Mir中，接下来判断
        bingqu = con1->GetExecutingRequest();
        if(bingqu.isEmpty())
        {
            //信号
            emit CmdSMFinished(SM_UPDATE_REQUEST ,false , QStringLiteral("no request execing"));
        }
        else
        {
            dest = con1->GetExecutingDest(bingqu);
            if(!dest.isEmpty())
            {
                //状态的更新要分请求类型进行讨论

                if(con1->GetRequestType(bingqu) == 4)   /*充电站的请求类型*/
                {
                    if(mirSchedule->state.battery_percentage >= 99)
                    {
                        mirSchedule->state.MIRTargetBingqu = dest;  //某个充电站
                        mirSchedule->state.MIRState = 2;            //已经充完的条件（和其它地点的到达不一样）
                        con1->SetDestState(bingqu,dest,2);
                        con1->SetGoCmdNext(bingqu);     /*充电请求的完成，意味着下一个请求自动执行*/
                        emit CmdSMFinished(SM_UPDATE_REQUEST ,true , QStringLiteral("charge complete"));
                    }
                    else
                    {
                        emit CmdSMFinished(SM_UPDATE_REQUEST ,true , QStringLiteral("charging"));
                    }
                }
                else            /*1,2,3正常处理*/
                {
                    //更改正在执行请求的正执行目的地的distance
                    if(con1->CalculateDistance(bingqu,dest,
                                               mirSchedule->state.pos_x,    //动态更新的MIR坐标
                                               mirSchedule->state.pos_y))
                    {
                        if(con1->GetDistance(bingqu,dest) < 1.0)
                        {
                            //更新mirSchedule的状态
                            mirSchedule->state.MIRTargetBingqu = dest;
                            mirSchedule->state.MIRState = 2;
                            //真正的队列执行
                            qDebug()<<"-------"<<bingqu<<dest;
                            con1->SetDestState(bingqu,dest,2);
                            emit CmdSMFinished(SM_UPDATE_REQUEST ,true , QStringLiteral("one dest reached"));
                        }
                        else
                        {
                            emit CmdSMFinished(SM_UPDATE_REQUEST ,true , QStringLiteral("one dest distance update"));
                        }
                    }
                }
            }
            else
                emit CmdSMFinished(SM_UPDATE_REQUEST ,false , QStringLiteral("no dest execing"));
        }
        break;                             //结束执行
    default:
        break;
    }
}

/*
 * 如果请求下发已经正确，但是执行过程中出现了不可描述的错误
 * 此机制，根据执行时间，超出一定时间内，认为出现问题了，
 * 对象 ：执行中的dest
 * 1,重新下发任务
 * 2,ready
*/
void CmdStateMachine::FixRequest()
{
    switch (appState) {
    case 0:
        qDebug()<<"FixRequest()：stateid"<<mirSchedule->state.stateText;
        if(mirSchedule->state.stateText == QStringLiteral("Error"))
        {
            qDebug()<<"FixRequest()： state== error";
            appState = 1;   /*清除错误*/
            FixRequest();
        }
        else
        {
            appState = 2;   /*没有错误，就执行普通任务的修理*/
            FixRequest();
        }
        break;
    case 1:
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            qDebug()<<"FixRequest()-type"<<type<<"result"<<result;
            if(type == 8 && result == true)
            {
                qDebug()<<"FixRequest() ClearError success";
                emit CmdSMFinished(SM_RETRY_REQUEST ,true , "clear error success" + detial);
            }
            else
            {
                qDebug()<<"FixRequest() ClearError failed";
                emit CmdSMFinished(SM_RETRY_REQUEST ,false , "clear error failed" + detial);
            }
        });
        con2->ClearError();
        break;
    case 2:
        if(!con1->IsQueueEmpty())
        {
            bingqu = con1->GetExecutingRequest();
            if(bingqu.isEmpty())    //没有执行中的请求
            {
                emit CmdSMFinished(SM_RETRY_REQUEST ,true , "no operation");
            }
            else                    //有执行中的请求
            {
                dest = con1->GetExecutingDest(bingqu);
                if(!dest.isEmpty())    //有未执行的目的地
                {
                    /*先累积时间*/
                    con1->PlusDestTick(bingqu,dest);

                    /*再判断时间*/
                    if(con1->GetDestTick(bingqu,dest) > 150) /*150 * 5s*/
                    {
                        con1->ResetDestTick(bingqu,dest);   /*清空累积时间*/
                        qDebug()<<"FixRequest() normal mission timeout";
                        appState = 3;                       /*流程跳转*/
                        FixRequest();
                    }
                    else
                    {
                        emit CmdSMFinished(SM_RETRY_REQUEST ,false , "no operation");
                    }
                }
                else        //没有未执行的目的地
                {
                    emit CmdSMFinished(SM_RETRY_REQUEST , true , "no operation");
                }

            }
        }
        else
        {
            //无请求 此处有信号
            emit CmdSMFinished(SM_RETRY_REQUEST , true , "no operation");
        }
        break;
    case 3: /*清空队列中因为异常没有完成的任务*/
        connect(con3,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 3 && result == true)
            {
                appState = 4;
                FixRequest();  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_RETRY_REQUEST ,false , "retry clear queue failed");
            }
        });
        con3->DeleteQueueFunc();
        break;
    case 4:
        connect(con4,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 2 && result == true)
            {
                appState = 5;
                FixRequest();  //递归 在函数体外
            }
            else
            {
                emit CmdSMFinished(SM_RETRY_REQUEST ,false , "retry execTask failed");
            }
        });
        con4->ExecTask(dest);
        break;                      //执行
    case 5:
        connect(con5,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 5 && result == true)
            {
                /*此举指示保证，一定要让MIR运行*/
                qDebug()<<"FixRequest（）--ready";
                emit CmdSMFinished(SM_RETRY_REQUEST ,true , "retry ready success");
            }
            else
            {
                emit CmdSMFinished(SM_RETRY_REQUEST ,false , "retry ready failed");
            }
        });
        con5->StartMirExecuting();
        break;//结束执行
    default:
        break;
    }
}
/*
 * 根据当前电量是否小于30，来插入充电任务命令
 * 如果有任务就不插入了，因为普通任务的优先级更高
 * 如果没有任务，那么就直接插入充电任务命令，由于充电任务是自发执行的，所以gocmd必须等于1
 *
*/
void CmdStateMachine::ChargeRequest()
{
    switch (appState) {
    case 0:
        if(!con1->IsQueueEmpty())           /*任务队列中有任务（优先级高），此时不可进行充电*/
        {   /**/
            emit CmdSMFinished(SM_CHARGE_REQUEST ,true , QStringLiteral("has mission ，charge can not insert"));
        }
        else
        {
            //任务没有了，电量低于20%就进行充电，高于20%则无动作
            if(mirSchedule->state.battery_percentage < 30)
            {
                /*后续插入充电站容器，注意name必须为充电站*/
                /*充电站无论何时都是自动前往的，所以它的发车gocmd必须为1*/
                if(con1->InsertInitialRequest(QStringLiteral("充电站"),QStringLiteral(""))) //初始请求
                {
                    con1->SetRequestGoCmd(QStringLiteral("充电站"),1);      //执行！
                    emit CmdSMFinished(SM_CHARGE_REQUEST ,true , QStringLiteral("charge insert"));
                }
                else
                    emit CmdSMFinished(SM_CHARGE_REQUEST ,false , QStringLiteral("charge insert error"));
            }
            else
            {
                emit CmdSMFinished(SM_CHARGE_REQUEST , true , "no operation");
            }
        }
        break;
    default:
        break;
    }
}
/*
 * 先获得position列表，然后根据position列表更新name x y
 *
*/
void CmdStateMachine::GetAllPosition()
{
    switch (appState) {
    case 0:
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                this,[=](int type, bool result, QString detial){
            if(type == 7 && result == true)
            {
                appState = 1;
                qDebug()<<detial;
                GetAllPosition();  //递归 继续执行
            }
            else
            {
                emit CmdSMFinished(SM_GET_ALL_POSITION ,false , QStringLiteral("get position failed"));
            }
        });
        con2->GetPositionFunc();
        break;                      //结束执行
    case 1:
        //由于不定回复，所以不进行判断了，由时间保证析构
        con3->GetAllPositionXYFunc();
        break;                             //结束执行
    case 2:
        break;
    default:
        break;
    }
}

void CmdStateMachine::GetMission()
{
    switch (appState) {
    case 0:
        if(con2 == nullptr)
            return;
        connect(con2,&MIRCmdCON::MIRCmdCONFinished,
                [=](int type, bool result, QString detial){
            qDebug()<<"type"<<type;
            if(type == 1 && result == true)
            {
                qDebug()<<detial;
                emit CmdSMFinished(SM_GET_MISSION ,true , QStringLiteral("get mission success"));
            }
            else
            {
                emit CmdSMFinished(SM_GET_MISSION ,false , QStringLiteral("get mission failed"));
            }
        });
        con2->GetMissionList();
        break;                      //结束执行
    default:
        break;
    }
}

void CmdStateMachine::GoCmdRequest()
{
    QString bingqu;
    bingqu = con1->GetExecutingRequest();
    if(bingqu.isEmpty())
    {
        bingqu = con1->GetFirstNotExecuteRequest();
        if(bingqu.isEmpty())
        {
            //不处理
            emit CmdSMFinished(SM_GO_CMD_REQUEST ,true , QStringLiteral("set request go failed"));
        }
        else
        {
            con1->SetRequestGoCmd(bingqu,1);
            emit CmdSMFinished(SM_GO_CMD_REQUEST ,true , QStringLiteral("set request go"));
        }
    }
    else
    {
        con1->SetRequestGoCmd(bingqu,1);
        emit CmdSMFinished(SM_GO_CMD_REQUEST ,true , QStringLiteral("set request go"));
    }
}

void CmdStateMachine::OpenCmdRequest()
{
    QString bingqu;
    bingqu = con1->GetExecutingRequest();
    if(bingqu.isEmpty())
    {
        bingqu = con1->GetFirstNotExecuteRequest();
        if(bingqu.isEmpty())
        {
            //不处理
            emit CmdSMFinished(SM_OPEN_CMD_REQUEST ,true , QStringLiteral("request opened failed"));
        }
        else
        {
            con1->SetRequestOpenedFlag(bingqu,1);
            emit CmdSMFinished(SM_OPEN_CMD_REQUEST ,true , QStringLiteral("request opened OK"));
        }
    }
    else
    {
        con1->SetRequestOpenedFlag(bingqu,1);
        emit CmdSMFinished(SM_OPEN_CMD_REQUEST ,true , QStringLiteral("request opened OK"));
    }
}
