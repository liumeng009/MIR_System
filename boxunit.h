#ifndef BOXUNIT_H
#define BOXUNIT_H

#include <QObject>
#include <QTimer>
#include "schedule/cmdstatemachine.h"
#include "schedule/mirschedule.h"
#include "applyform/applyform.h"
#include "applyform/applyjson.h"
#include "sql/boxtablecon.h"

/*
 * 箱体类代表一个箱体对象，作为整个系统的实体类
 * 依赖：1.MIRSchedule实例：一个绑定的调度系统（因为每个箱体都是单独调度的）
 *      2.BoxTableCON实例：箱体类要操作数据库表（单线程访问，无数据一致性问题）
*/
class BoxUnit : public QObject
{
    Q_OBJECT
public:
    explicit BoxUnit(QObject * parent = nullptr);

    //单例依赖（一个boxUnit包含一个mirSchedule调度）
    MIRSchedule * mirSchedule = nullptr;
    BoxTableCON * boxTableCON = nullptr;

    //成员
    int boxNum = 0;
    QString ip;
    QString port;
    int goCmd = 0;
    int opened = 0;
    QTimer * timer = nullptr;   //处理定时器
    QTimer * timer1 = nullptr;  //更新定时器
    QTimer * timer2 = nullptr;  //守护定时器
    QTimer * timer3 = nullptr;  //重试定时器

    //成员函数
    /*设置箱体信息*/
    void SetBoxNum(int num);
    void SetGoCmd(int cmd);
    void SetOpened();
    void ResetOpened();

    /*构建和析构调度系统*/
    void BuildMirSchedule(QString missionName, QString chargeMissionName);
    void DeleteMirSchedule();

    /*调度系统内容更新与复位*/
    bool UpdateMirSchedule();
    bool ResetMirSchedule();

    /*库存操作*/
    QString GetAbleStock();

    void RealStockUnitToRealStockJson(); //mem -> DB
    void RealStockJsonToRealStockUnit();   //DB - > mem

    void AbleStockUnitToAbleStockJson(); //mem -> TCP
    void ApplyFormJsonToApplyFormUnit(); //TCP -> mem


    void StockJsonToApplyForm(ApplyJson * ApplyJson ,ApplyUnit * applyUnit);
    void ApplyFormToApplyJson(ApplyUnit *applyUnit, ApplyJson *applyJson);
signals:
    /*用以接收回传的信号*/
    void OperationFinished(int type, bool result, QString detial);
    void DataChanged();             /*当数据发生更新*/
public slots:
    void CmdSMFinished(CmdStateMachine * sm, int type, bool result, const QString &detial);

};

class BoxUnitFactory
{
public:
    static BoxUnit * instance(QObject *parent = nullptr)
    {
        return new BoxUnit(parent);;
    }
};

#endif // BOXUNIT_H
