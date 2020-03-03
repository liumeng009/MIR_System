#ifndef MIRCMDCON_H
#define MIRCMDCON_H

#include <QObject>
#include "mirschedule.h"
#include "httpcon.h"

/*      MIRCmd++++++>
 *                HttpCON++++++>
 *                              baseAPI
 *                HttpCON<-----
 *      MIRCmd<------
 *  (MIR命令函数)  (构建REST接口)  (http收发)
 *
 * MIRCmdCON 依赖于 MIRSchedule，要从调度系统中取得 IP 和 各种MIR的内部参数
*/

class MIRCmdCON : public QObject
{
    Q_OBJECT
public:
    explicit MIRCmdCON(QObject *parent = nullptr,
                       MIRSchedule * mirSchedule = nullptr);

    //依赖
    MIRSchedule * mirSchedule = nullptr;    //MIR所有数据

    //成员
    void AbortMirExecuting();
    void StartMirExecuting();
    void DeleteMirQueue();
    void GetMirState();
    void ExecTask(QString dest);
    void GetMissionList();
    void GetPositionList();
    void ClearError();

    QString error;
    QString dest;

    /* 每一个对象只进行一种操作 , 用完对象就释放掉了 */
    //回传的是执行成功或失败,数据的话已经可以直接刷新了

    /*基础调用API 实例化httpCon对象，不同操作 不同回应*/
    int ReplayStatusCode(QNetworkReply *reply);

    void PostMissionFunc(const QString url,
                         const QString groupID,
                         const QString name);
    void GetMissionFunc();

    void PostMissionQueueFunc(const QString message,
                              const QString missionID,
                              const QString array1,
                              const QString array2,
                              const float priority);

    void DeleteQueueFunc();
    void GetMissionQueueFunc(const QString url);

    void GetStatusFunc();
    void PostStatusFunc(const QString state_id);
    void PostStatusClearErrorFunc();

    void GetPositionFunc();
    void GetPositionXYFunc(const QString guid);
    void GetAllPositionXYFunc();
    /*
     * GetMission           1
     * PostMissionQueue     2
     * DeleteQueue          3
     * GetStatus            4
     * PostStatus           5
     * GetPosition          6
     * GetPositionXY        7
     * PostStatusClearError 8
    */

signals:
    void MIRCmdCONFinished(int type, bool result, const QString detial);

public slots:
    /*ReplyProcee每当收到一个http response，那么就执行一次
    reply中含有url，可以定位到具体的机器人。
    具体有5中reply，对应这5个命令，因为每个请求的格式不同，参数不同需要单独处理
    1.首先解析reply，这个在接收函数中已经完成
    2.然后遍历robotlist，查找url相同的那个，将结果填入MIR*/

    void PostMissionReply(QNetworkReply *reply);
    void GetMissionReply(QNetworkReply *reply);
    void PostMissionQueueReply(QNetworkReply *reply);
    void GetMissionQueueReply(QNetworkReply *reply);
    void DeleteQueueFuncReply(QNetworkReply *reply);
    void GetStatusReply(QNetworkReply *reply);
    void PostStatusReply(QNetworkReply *reply);
    void PostStatusClearErrorReply(QNetworkReply *reply);
    void GetPositionReply(QNetworkReply *reply);
    void GetPositionXYReply(QNetworkReply *reply);
};

class MIRCmdCONFactory
{
public:
    static MIRCmdCON * instance(QObject *parent = nullptr,
                             MIRSchedule * mirSchedule = nullptr)
    {
        return new MIRCmdCON(parent,mirSchedule);;
    }
};


#endif // MIRCMD_H
