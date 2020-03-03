#include "rms.h"
#include "rmsfactory.h"
#include <QHostInfo>
#include <QSqlQuery>

RMS::RMS(QObject *parent)
    : QObject(parent)
    ,bingquTableCON(BingquTableCONFactory::instance(this))
    ,boxTableCON(BoxTableCONFactory::instance(this))
    ,recordTableCON(RecordTableCONFactory::instance(this))
    ,yihuTableCON(YihuTableCONFactory::instance(this))
    ,sysConfig(SysConfigFactory::instance(this))
    ,excelEngine(ExcelEngine::getInstance())
{
    /*实时工厂实例建立之后，建立工厂,以便在其他模块调用RMSFactory()*/
    SetRMSFactory(this);

    /*数据表模块，主要用来读取数据表*/
    bingquModule = new SqlModule("病区表");
    yihuModule = new SqlModule("医护表");
    zhiwenModule = new SqlModule("指纹表");
    xiangtiModule = new SqlModule("箱体配置表");
    /*配置文件跟新到DB*/
    UpdateStockFromConfig();

    /*从mem箱体配置表中更新boxmap*/
    UpdateFromBoxTable();

    /*系统表格，采集系统所有重要数据*/
    //tableInit();
    dataTable = new Table(this);
    /*设置record list 只读*/
    dataTable->SetBingquTable(bingquModule->GetRecordList());
    dataTable->SetYihuTable(yihuModule->GetRecordList());
    dataTable->SetFingerTable(zhiwenModule->GetRecordList());
    dataTable->SetXiangtiTable(xiangtiModule->GetRecordList());

    /*dataTable获得指纹表的module 可读可写，主要为了写入指纹*/
    /*如果其他表需要写，也可以继续添加*/
    dataTable->SetFingerModule(zhiwenModule);

    //CreateTable();
    //InsertData();
    //UpdateData();
    //SelectData();

    /*普通定时器*/
    demoTimer = new QTimer(this);
    QObject::connect(demoTimer,SIGNAL(timeout()),this,SLOT(TimerCallBack()));
    demoTimer->start(1000);

    /*TCP服务器，用于处理所有socket连接，数据发送与接收*/
    demoTcpServer = new TcpServer(this);
    demoTcpServer->SetBoxList(GetBoxList());
    demoTcpServer->SetTcpList(GetTcpList());
    demoTcpServer->listen(QHostAddress::Any, 8000); /*监听端口*/
    //dataTable->SetSocketTable(GetSocketList());
    qDebug()<<demoTcpServer->errorString();
    qDebug()<<demoTcpServer->serverAddress();
    qDebug()<<demoTcpServer->serverPort();
    //qDebug()<<demoTcpServer->proxy().hostName();

    chatServer = ChatServerFactory::instance(this);
    chatServer->toggleStartServer();


    /*串口和会话的实例化,在界面初始化之后*/
    session = new Session(this); //是mSerialport的上层

    /*显示系统*/
    demoWindow = new RmsWindow(this);

    demoWindow->SetBingquModelView(bingquModule->GetDataModel(),
                                   bingquModule->GetDataView());
    demoWindow->SetYihuModelView(yihuModule->GetDataModel(),
                                 yihuModule->GetDataView());
    demoWindow->SetBoxModelView(xiangtiModule->GetDataModel(),
                                xiangtiModule->GetDataView());
    demoWindow->AddViewFunc();



    /*串口界面与底层驱动的耦合*/
    demoWindow->fingerToolWindow->SetSession(session);
}

BoxUnit * RMS::GetBoxUnit(int boxNum)
{
    if(boxMap.contains(boxNum))
        return boxMap.value(boxNum);
    else
        return nullptr;
}
QStringList RMS::GetAllBoxKeys(){
    QMap<int,BoxUnit*>::iterator it;
    QStringList result;
    if(boxMap.isEmpty()){
        //nothing to do
    }
    else{
        for(it = boxMap.begin(); it != boxMap.end(); it++){
            result << QString::number(it.key());
        }
    }
    return result;
}
void RMS::UpdateFromBoxTable()
{
    QMap<int,BoxUnit*>::iterator it;
    if(boxMap.size() != 0)
    {
        //首先删除map中的不存在项
        for(it = boxMap.begin(); it != boxMap.end();)
        {
            if(!boxTableCON->IsBoxExist(it.key()))
            {
                it = boxMap.erase(it);
            }
            else
                it++;
        }
    }

    //添加map中没有的项 && 更新map中的内容
    int boxNum = 0;
    QString ip;
    QString port;
    QSqlQuery query = boxTableCON->GetBoxRecord();
    while(query.next())
    {
        /*参数从DB中获取*/
        boxNum = query.value(0).toInt();
        ip = query.value(1).toString();
        port = query.value(2).toString();

        qDebug()<<boxNum;
        if(!boxMap.contains(boxNum))
        {
            qDebug()<<"no"<<boxNum;
            BoxUnit * pBoxUnit = nullptr;
            pBoxUnit = BoxUnitFactory::instance(this);
            pBoxUnit->SetBoxNum(boxNum);
            boxMap.insert(boxNum,pBoxUnit);
        }

        //更新数据 此刻已经有相关项了
        boxMap.value(boxNum)->ip = ip;
        boxMap.value(boxNum)->port = port;

        /*更新boxUNit的schedule的参数设置，没有实例化操作！！！*/
        /*只是设置taskName = mission_test*/
        boxMap.value(boxNum)->BuildMirSchedule(sysConfig->taskName,
                                               sysConfig->chargeTaskName);
    }

    /*已经实例化,更新视图*/
    if(demoWindow)
    {
        demoWindow->mainBoxWindow->UpdateListView();
    }
}
bool RMS::IsBoxIdExist(int boxid)
{
    return boxMap.contains(boxid);
}
void RMS::UpdateStockFromConfig()
{
    int boxNum = -1;
    QSqlQuery query = boxTableCON->GetBoxRecord();
    while(query.next())
    {
        boxNum = query.value(0).toInt();
        if(boxTableCON->IsStockEmpty(boxNum))
        {
            if(sysConfig->LoadJsonConfig(boxNum) == false)
            {
                qDebug()<<"load config file failed"<<boxNum;
                continue;
            }
            boxTableCON->SaveStock(boxNum,QString(sysConfig->GetJsonConfig().toJson()));
        }
        else
        {
            //不要变化，除非认为操作，强制更新
        }
    }
}
void RMS::TimerCallBack()
{
}

void RMS::SysTimerTask()
{
}

RMS::~RMS()
{
    qDebug()<<"rms close";
    /*所有服务器关闭*/
    demoTcpServer->close();
    chatServer->toggleStartServer();

    bingquModule->deleteLater();
    yihuModule->deleteLater();
    zhiwenModule->deleteLater();

    //delete mSerialPort;
    delete session;
}
