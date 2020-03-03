#ifndef REQUESTQUEUECON_H
#define REQUESTQUEUECON_H

#include <QObject>
#include "mirschedule.h"
#include "requestnode.h"

/* RequestQueueCON(  父引用（析构用）   ，   调度系统)
 *                                          |
 *                                          |
 *                          MIRSchedule（的任务命令队列）
 *
 * 此类对象操作的目标对象来自于调度系统的任务命令队列，
 *
 * 可以做进调度系统，但是那样耦合太严重，两方的自己的改动都会造成已有代码的大变动
 * 因此在构造函数中进行依赖注入，后者利用工厂方法和箱体编号进行依赖注入是有必要的。
 *
 * 对于行为类的话，可以利用接口，和基于接口的各种实现来达到应对需求变化的目的；
 */

class RequestQueueCON : public QObject
{
    Q_OBJECT
public:
    explicit RequestQueueCON(QObject *parent = nullptr,
                             MIRSchedule * mirSchedule = nullptr);

    //依赖
    MIRSchedule * mirSchedule = nullptr;

signals:
    void SignalTrue();
    void SignalFalse();
public slots:
    //成员
    //主要针对未完成的任务，已完成的任务基本不判断(或者可以删除)
    bool IsQueueEmpty();
    bool IsRequestExecuting(QString bingqu);
    bool IsRequestNotExecuting(QString bingqu);
    bool IsRequestOpened(QString bingqu);
    //集散地 IsRequestExecuting(”集散地“);

    bool RequestQueueClear();
    bool InsertRequest(QString bingqu,QString requestContent);
    bool InsertInitialRequest(QString bingqu, QString requestContent);

    bool SetRequestGoCmd(QString bingqu, int go);
    int  GetRequestGoCmd(QString bingqu);
    bool SetGoCmdNext(QString bingqu);

    bool SetRequestOpenedFlag(QString bingqu, int opened);
    int  GetRequestOpenedFlag(QString bingqu);

    int GetRequestType(QString bingqu);
    QString GetRequestContent(QString bingqu);

    bool DeleteRequest(QString bingqu);
    bool DeleteRequestWithGoCmdNext(QString bingqu);

    QString GetExecutingRequest();
    QString GetFirstNotExecuteRequest();
    bool HasNotexecDest(QString bingqu);
    bool HasExecDest(QString bingqu);
    bool AllDestComplete(QString bingqu);
    QString GetNotexecDest(QString bingqu);
    bool HasNotexecRequest();       /*存在没有执行的请求*/
    bool HasNotexecNormalRequest();       /*存在没有执行的请求(除了集散地)*/
    QString GetExecutingDest(QString bingqu);

    bool CalculateDistance(QString bingqu, QString dest, double x, double y);
    double GetDistance(QString bingqu, QString dest);


    bool SetDestState(QString bingqu, QString dest, int state);
    bool SetRequestState(QString bingqu, int state);
    int GetRequestState(QString bingqu);

    bool ResetDestTick(QString bingqu, QString dest);
    bool PlusDestTick(QString bingqu, QString dest);
    int GetDestTick(QString bingqu, QString dest);

    RequestNode * CreateRequestNode(MIRSchedule * parent = nullptr,
                                    QString bingqu = "",
                                    QString requestContent = "");
};

class RequestQueueCONFactory
{
public:
    static RequestQueueCON * instance(QObject *parent = nullptr,
                                      MIRSchedule * mirSchedule = nullptr)
    {
        return new RequestQueueCON(parent,mirSchedule);;
    }
};

#endif // REQUESTQUEUECON_H
