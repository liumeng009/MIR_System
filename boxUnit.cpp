#include "boxunit.h"
#include <QStringLiteral>

BoxUnit::BoxUnit(QObject *parent) : QObject(parent)
  ,mirSchedule(MIRScheduleFactory::instance(this))
  ,boxTableCON(BoxTableCONFactory::instance())
  ,timer(new QTimer(this))      /*普通处理*/
  ,timer1(new QTimer(this))     /*更新状态*/
  ,timer2(new QTimer(this))     /*任务列表 地址列表检查*/
  ,timer3(new QTimer(this))     /*任务列表 地址列表检查*/
{
    /*周期性5s进行一次调度系统的正常处理，这是箱体本身必须保证的*/
    connect(timer,&QTimer::timeout,
            this,[this](){

        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
        connect(sm,&CmdStateMachine::CmdSMFinished,
                this,std::bind(&BoxUnit::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        sm->ProcessRequest();
    });
    timer->start(1000);

    /*周期性3s进行一次调度系统的正常处理，这是箱体本身必须保证的*/
    connect(timer1,&QTimer::timeout,
            this,[this](){
        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
        connect(sm,&CmdStateMachine::CmdSMFinished,
                this,std::bind(&BoxUnit::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        sm->UpdateRequest();
    });
    timer1->start(3000);

    /*周期性30s进行一次箱体数据（包括调度系统）的检查，这是箱体本身必须保证的*/
    connect(timer2,&QTimer::timeout,
            this,[this](){
        /*参数守护机制*/
        CmdStateMachine * sm1 = CmdStateMachineFactory::instance(this,mirSchedule);
        if(sm1->IsMissionEmpty())
        {
            CmdStateMachine *sm2 = CmdStateMachineFactory::instance(this,mirSchedule);
            connect(sm2,&CmdStateMachine::CmdSMFinished,
                    this,std::bind(&BoxUnit::CmdSMFinished,this,sm2,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
            sm2->GetMission();
        }

        if(sm1->IsPositionEmpty())
        {
            CmdStateMachine *sm2 = CmdStateMachineFactory::instance(this,mirSchedule);
            connect(sm2,&CmdStateMachine::CmdSMFinished,
                    this,std::bind(&BoxUnit::CmdSMFinished,this,sm2,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
            sm2->GetAllPosition();
        }

        sm1->deleteLater(); //别忘了最后删除对象
    });
    timer2->start(30000);

    /*周期性5s进行一次，这是箱体本身必须保证的*/
    connect(timer3,&QTimer::timeout,
            this,[this](){
        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
        connect(sm,&CmdStateMachine::CmdSMFinished,
                this,std::bind(&BoxUnit::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        sm->FixRequest();

    });
    connect(timer3,&QTimer::timeout,
            this,[this](){
        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
        connect(sm,&CmdStateMachine::CmdSMFinished,
                this,std::bind(&BoxUnit::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        sm->ChargeRequest();

    });
    timer3->start(5000);
}
void BoxUnit::SetBoxNum(int num)
{
    boxNum = num;
}
void BoxUnit::SetGoCmd(int cmd)
{
    goCmd = cmd;

    if(cmd == 1)
    {
        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
        sm->GoCmdRequest(); //无需返回了
        sm->deleteLater();
        goCmd = 0;
    }
    else
    {

    }
}
void BoxUnit::SetOpened()
{
    opened = 1;
}
void BoxUnit::ResetOpened()
{
    opened = 0;
}

/*构建和析构调度系统*/
void BoxUnit::BuildMirSchedule(QString missionName,QString chargeMissionName)
{
    //取来自boxUnit的ip port
    mirSchedule->SetMIR(ip,port,missionName,chargeMissionName);

    //设置好ip port 之后
    CmdStateMachine * sm = CmdStateMachineFactory::instance(this,mirSchedule);
    connect(sm,&CmdStateMachine::CmdSMFinished,
            this,std::bind(&BoxUnit::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    sm->GetMission();

    /*重新实例化对象，用以单独处理position信息*/
    CmdStateMachine * sm1 = CmdStateMachineFactory::instance(this,mirSchedule);
    connect(sm1,&CmdStateMachine::CmdSMFinished,
            this,std::bind(&BoxUnit::CmdSMFinished,this,sm1,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    sm1->GetAllPosition();
}
void BoxUnit::DeleteMirSchedule()
{

}
/*
 * stockJson 转化为 applyForm 用于操作和计算
*/
void BoxUnit::StockJsonToApplyForm(ApplyJson * ApplyJson ,ApplyUnit * applyUnit)
{
    bool result;
    QStringList keys;
    QJsonDocument jsonDmt;
    QJsonObject jBoxObj,jDrawerObj,jCellObj;
    QJsonArray subArray,thirdArray;
    DrawerUnit * pDrawer;
    CellUnit * pCell;

    //清空applyform 注意！
    applyUnit->ClearData();

    //解析JSON文件 jBoxObj
    jBoxObj = ApplyJson->ApplyJsonMessage;

    //开始输出为applyForm
    applyUnit->bingqu = QStringLiteral("");
    jBoxObj.value(QStringLiteral("bingqu")).toString();
    applyUnit->boxNum = jBoxObj.value(QStringLiteral("boxNum")).toInt();
    applyUnit->MIRIP = jBoxObj.value(QStringLiteral("MIR_IP")).toString();
    applyUnit->MIRPORT = jBoxObj.value(QStringLiteral("MIR_PORT")).toInt();
    applyUnit->enabled = jBoxObj.value(QStringLiteral("enabled")).toInt();
    applyUnit->checked = jBoxObj.value(QStringLiteral("checked")).toInt();

    //如果有drawers
    if(jBoxObj.contains("drawers"))
    {
        //提取第二层 得到数组-》遍历数组-》得到内容-》添加到applyForm
        subArray = jBoxObj.value("drawers").toArray();

        if(subArray.isEmpty())
            return;

        for(int i = 0; i< subArray.size(); i++)
        {
            jDrawerObj = subArray.at(i).toObject();

            //obj数据提取
            pDrawer = DrawerUnitFactory::instance();
            pDrawer->DrawerNum = jDrawerObj.value("drawerNum").toInt();
            pDrawer->layoutFile = jDrawerObj.value("layoutFile").toString();
            pDrawer->categoryName = jDrawerObj.value("categoryName").toString();
            pDrawer->enabled = jDrawerObj.value("enabled").toInt();
            pDrawer->checked = jDrawerObj.value("checked").toInt();
            applyUnit->DrawerUnitList.append(pDrawer);

            //子循环提取第三层 得到数组-》遍历数组-》得到内容-》添加到applyForm
            if(jDrawerObj.contains("cells"))
            {
                thirdArray = jDrawerObj.value("cells").toArray();

                if(thirdArray.isEmpty())
                    return;

                for(int i = 0; i< thirdArray.size(); i++)
                {
                    jCellObj = thirdArray.at(i).toObject();

                    //obj数据提取
                    pCell = CellUnitFactory::instance();
                    pCell->CellNum = jCellObj.value("CellNum").toInt();
                    pCell->pos_leftup_X = jCellObj.value("pos_leftup_X").toInt();
                    pCell->pos_leftup_Y = jCellObj.value("pos_leftup_Y").toInt();
                    pCell->widget_width = jCellObj.value("widget_width").toInt();
                    pCell->widget_height = jCellObj.value("widget_height").toInt();
                    pCell->specsName = jCellObj.value("specsName").toString();
                    pCell->stockAmount = jCellObj.value("stockAmount").toInt();
                    pCell->increaseAmount = jCellObj.value("increaseAmount").toInt();
                    pCell->estimateStockAmount = jCellObj.value("estimateStockAmount").toInt();
                    pCell->referStockAmount = jCellObj.value("referStockAmount").toInt();
                    pCell->supplyAmount = jCellObj.value("supplyAmount").toInt();
                    pCell->lackAmount = jCellObj.value("lackAmount").toInt();
                    pCell->lackState = jCellObj.value("lackState").toInt();
                    pCell->enabled = jCellObj.value("enabled").toInt();
                    pCell->checked = jCellObj.value("checked").toInt();

                    pDrawer->CellUnitList.append(pCell);
                }
            }
        }
    }
    qDebug()<<"end";
    //将解析后的文件生成applyform
}

/*
 * applyForm 转化为 stockJson 用于操作和计算
*/
void BoxUnit::ApplyFormToApplyJson(ApplyUnit * applyUnit,
                                   ApplyJson * applyJson)
{
    QJsonObject jBoxObj,jDrawerObj,jCellObj;    //箱体，抽屉，单元格
    QJsonArray subArray,thirdArray; //第二层、第三层


    //从applyForm输出Json object
    jBoxObj.insert("bingqu", QJsonValue(applyUnit->bingqu));
    jBoxObj.insert("boxNum", QJsonValue(applyUnit->boxNum));
    jBoxObj.insert("MIR_IP", QJsonValue(applyUnit->MIRIP));
    jBoxObj.insert("MIR_PORT", QJsonValue(applyUnit->MIRPORT));
    jBoxObj.insert("enabled", QJsonValue(applyUnit->enabled));
    jBoxObj.insert("checked", QJsonValue(applyUnit->checked));

    //遍历applyUnit的drawerlist
    if(!applyUnit->DrawerUnitList.isEmpty())
    {
        for(int i = 0; i < applyUnit->DrawerUnitList.size(); i++)
        {
            jDrawerObj = QJsonObject(); //清空

            jDrawerObj.insert("drawerNum",
                        applyUnit->DrawerUnitList.at(i)->DrawerNum);
            jDrawerObj.insert("layoutFile",
                        applyUnit->DrawerUnitList.at(i)->layoutFile);
            jDrawerObj.insert("categoryName",
                        applyUnit->DrawerUnitList.at(i)->categoryName);
            jDrawerObj.insert("enabled",
                        applyUnit->DrawerUnitList.at(i)->enabled);
            jDrawerObj.insert("checked",
                        applyUnit->DrawerUnitList.at(i)->checked);

            //处理cells后 才算处理完
            thirdArray = QJsonArray();  //清空
            if(!applyUnit->DrawerUnitList.at(i)->CellUnitList.isEmpty())
            {
                for(int j = 0; j < applyUnit->DrawerUnitList.at(i)->CellUnitList.size(); j++)
                {
                    jCellObj = QJsonObject();       //清空

                    jCellObj.insert("CellNum",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->CellNum);
                    jCellObj.insert("pos_leftup_X",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->pos_leftup_X);
                    jCellObj.insert("pos_leftup_Y",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->pos_leftup_Y);
                    jCellObj.insert("widget_width",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->widget_width);
                    jCellObj.insert("widget_height",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->widget_height);
                    jCellObj.insert("specsName",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->specsName);
                    jCellObj.insert("stockAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->stockAmount);
                    jCellObj.insert("increaseAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->increaseAmount);
                    jCellObj.insert("estimateStockAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->estimateStockAmount);
                    jCellObj.insert("referStockAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->referStockAmount);
                    jCellObj.insert("supplyAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->supplyAmount);
                    jCellObj.insert("lackAmount",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->lackAmount);
                    jCellObj.insert("lackState",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->lackState);
                    jCellObj.insert("enabled",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->enabled);
                    jCellObj.insert("checked",
                                applyUnit->DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->checked);

                    thirdArray.append(jCellObj);
                }
            }
            else
            {
                //没有就不添加了
            }
            jDrawerObj.insert("cells",thirdArray);

            subArray.append(jDrawerObj);
        }
    }
    else
    {
        //空则不添加
    }
    jBoxObj.insert("drawers", subArray);

    /*更新applyjson*/
    applyJson->ApplyJsonMessage = jBoxObj;
}

/*
 * 获得可以申请的物品清单
 * 1.获得数据库中的实际库存json
 * 2.获得队列中已经申请的单子 json
 * 3.json转化为unit，进行想减
 * 4.再次转化为json-》string
*/
QString BoxUnit::GetAbleStock()
{
    //从数据库中获得的真实变量
    ApplyUnit * stockUnit = new ApplyUnit(this);
    ApplyJson * stockJson = new ApplyJson(this);

    //从队列中获取的临时数据
    ApplyUnit * requestUnit = new ApplyUnit(this);
    ApplyJson * requestJson = new ApplyJson(this);
    /*先计算*/
    //qDebug()<<"apply unit create"<<QString::number((unsigned int)stockUnit,16);

    QStringList requestContentList;

    //DB string -> mem json :real stock utf8 实际库存
    stockJson->QstringToJson(boxTableCON->GetStock(boxNum));

    //mem json -> mem unit :real stock
    //StockJsonToApplyForm(stockJson,stockUnit); //问题
    stockUnit->FromJson(stockJson->ApplyJsonMessage);

    //未执行完的contentlist
    requestContentList = mirSchedule->GetRequestContent();
    foreach (QString item, requestContentList) {
        requestJson->QstringToJson(item);
        requestUnit->FromJson(requestJson->ApplyJsonMessage);

        if(!stockUnit->Minus(requestUnit))
            return QStringLiteral("");
    }

    //再回复
    QString result = QString(QJsonDocument(stockUnit->ToJson()).toJson());

    stockUnit->deleteLater();
    requestUnit->deleteLater();
    stockJson->deleteLater();
    requestJson->deleteLater();

    return result;
}

void BoxUnit::CmdSMFinished(CmdStateMachine * sm, int type, bool result, const QString &detial)
{
    switch (type) {
    case SM_UPDATE_REQUEST:
        if(result)
        {
        }
        else
        {
        }
        break;
    case SM_PROCESS_REQUEST:
        if(result)
        {
        }
        else
        {
        }
        break;
    case SM_RETRY_REQUEST:
        if(result)
        {
        }
        else
        {
        }
        break;
    default:
        break;
    }

    //不判断，直接析构，因为更改已经完成
    sm->deleteLater();
}
