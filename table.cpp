#include "table.h"
#include "frame.h"
#include "httplist.h"
#include "rmsfactory.h"

int loginValid = 0;
/*密码*/
QString manaCode;
QString opCode;
QString openCode;

/*有效任务名称*/
QString taskName;

/*叫车服务*/
int callForMir;    //请求标志位0：无 1：有
QString calledMir; //具体信息 flag0：哪个机器人被叫了 flag1：无意义

/*操作类 class table class tcpServer */
QList <BoxItem> boxList; /*箱体--机器人参数 的表格*/
QList <TcpItem> socketList; /*socket管理表格的实例*/
QList <DATA_SYS_DATA> sysStateTable; /*系统状态表,显示用*/

Table::Table(QObject * parent): QObject(parent)
  ,bingquTableCON(BingquTableCONFactory::instance())
  ,yihuTableCON(YihuTableCONFactory::instance())
{
    /*原先代码，用于更新到各个DB表到指定的table中*/
    dateTime = new QDateTime(QDateTime::currentDateTime());
    demoTimer = new QTimer(this);
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(BingquTableUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(YihuTableUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(FingerTableUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(XiangtiTableUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(BoxListUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(ConfigUpdate()));

    connect(demoTimer,SIGNAL(timeout()),this,SLOT(SysStatusTableUpdate()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(CallMirStatusUpdate()));

    demoTimer->start(5000); /*周期5s 但愿不要冲突*/

    dataTimer = new QTimer(this);
    connect(dataTimer,SIGNAL(timeout()),this,SLOT(SocketTaskProcess()));
    connect(dataTimer,SIGNAL(timeout()),this,SLOT(RxBuff2TcpBuff()));
    connect(dataTimer,SIGNAL(timeout()),this,SLOT(TcpBuffVertify()));
    dataTimer->start(10); /*周期10ms 但愿不要冲突*/
    
    configIni = new QSettings("config.ini", QSettings::IniFormat);
    manaCode = configIni->value("manage").toString();
    opCode = configIni->value("operation").toString();
    openCode = configIni->value("open").toString();
}
Table::~Table()
{
    if(!boxList.isEmpty())
    {
        for(int i = 0; i < boxList.size(); i++)
        {
            boxList.at(i).expressList->clear();
            boxList.at(i).missionGuidList->clear();
            boxList.at(i).positonGuidList->clear();
            boxList.at(i).casheGuidList->clear();
            delete boxList.at(i).expressList;
            delete boxList.at(i).missionGuidList;
            delete boxList.at(i).positonGuidList;
            delete boxList.at(i).casheGuidList;
        }
        boxList.clear();
    }

    if(!socketList.isEmpty())
    {
        for(int i = 0; i < socketList.size(); i++)
        {
            socketList.at(i).tcpSocketPtr->deleteLater();
        }
        socketList.clear();
    }
}
/*根据本地配置更新密码*/
void Table::ConfigUpdate()
{
    manaCode = configIni->value("manage").toString();
    opCode = configIni->value("operation").toString();
    openCode = configIni->value("open").toString();
    taskName = configIni->value("taskName").toString();
}


/*设置数据库的指针，仅操作一次*/
void Table::SetBingquTable(QList<QSqlRecord> * demoPtr)
{
    bingquRecordList = demoPtr;
}
void Table::SetYihuTable(QList<QSqlRecord> * demoPtr)
{
    yihuRecordList = demoPtr;
}
void Table::SetFingerTable(QList<QSqlRecord> * demoPtr)
{
    fingerRecordList = demoPtr;
}
void Table::SetXiangtiTable(QList<QSqlRecord> * demoPtr)
{
    xiangtiRecordList = demoPtr;
}
void Table::SetFingerModule(SqlModule * demoPtr)
{
    fingerModule = demoPtr;
}
/*以下数据库更新到内存*/
void Table::BingquTableUpdate()
{
    DATA_BINGQU temp;
    QString abc;
    QByteArray array;
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");

    if(bingquRecordList->size() == 0)
        return;

    //先清空
    bingquTable.clear();
    for(int i = 0; i !=  bingquRecordList->size(); i++)
    {
        temp.id = bingquRecordList->at(i).value("id").toInt();

        abc = bingquRecordList->at(i).value("name").toString();
        temp.strName = abc;                         //保存QString形式

        array = abc.toLocal8Bit();
        if(array.size() > 8)
            array.resize(8);

        abc = gbk->toUnicode(array);    //保存GBK形式

        if(abc.toLocal8Bit().size() > 16)
            abc.clear();

        memset(temp.name,0,16);
        memcpy(temp.name,
               abc.toLocal8Bit().data(),
               abc.toLocal8Bit().size());

        //memcpy(temp.name,
        //       bingquRecordList->at(i).value("name").toByteArray().data(),
        //       bingquRecordList->at(i).value("name").toByteArray().size());
        bingquTable.append(temp);
        //qDebug()<<bingquRecordList->at(i).value("name").toByteArray();
    }
}
void Table::YihuTableUpdate()
{
    DATA_YIHU temp;
    QString abc;
    QByteArray array;
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");


    if(yihuRecordList->size() == 0)
        return;

    //先清空
    yihuTable.clear();
    for(int i = 0; i !=  yihuRecordList->size(); i++)
    {
        temp.id = yihuRecordList->at(i).value("id").toInt();

        abc = yihuRecordList->at(i).value("name").toString();
        temp.strName = abc;                         //保存QString形式
        abc = gbk->toUnicode(abc.toLocal8Bit());    //保存GBK形式

        array = abc.toLocal8Bit();
        if(array.size() > 8)
            array.resize(8);

        abc = gbk->toUnicode(array);    //保存GBK形式

        if(abc.toLocal8Bit().size() > 8)
            abc.clear();

        memset(temp.name,0,8);
        memcpy(temp.name,
               abc.toLocal8Bit().data(),
               abc.toLocal8Bit().size());

        //更新病区id
        temp.bingquId = yihuRecordList->at(i).value("bingquid").toInt();
        //更新指纹code
        temp.strFinger = yihuRecordList->at(i).value("finger").toString();

        yihuTable.append(temp);
    }

}
void Table::FingerTableUpdate()
{
    DATA_FINGER temp;

    if(fingerRecordList->size() == 0)
        return;

    //先清空
    fingerTable.clear();
    for(int i = 0; i !=  fingerRecordList->size(); i++)
    {
        temp.id = fingerRecordList->at(i).value("id").toInt();
        /*指纹数据提取*/
        fingerTable.append(temp);
    }
}
/*将数据库的箱体配置表，更新到内存中*/
void Table::XiangtiTableUpdate()
{
    DATA_XIANGTI temp;

    if(xiangtiRecordList->size() == 0)
        return;

    //先清空
    xiangtiTable.clear();
    for(int i = 0; i !=  xiangtiRecordList->size(); i++)
    {
        temp.id = xiangtiRecordList->at(i).value("id").toString();
        temp.ip = xiangtiRecordList->at(i).value("ip").toString();
        temp.port = xiangtiRecordList->at(i).value("port").toString();
        /*
        temp.id = xiangtiRecordList->at(i).value("id").toInt();
        memcpy(temp.ip,
               xiangtiRecordList->at(i).value("ip").toByteArray().data(),
               xiangtiRecordList->at(i).value("ip").toByteArray().size());
        memcpy(temp.port,
               xiangtiRecordList->at(i).value("port").toByteArray().data(),
               xiangtiRecordList->at(i).value("port").toByteArray().size());
        */
        xiangtiTable.append(temp);
    }
}

/*将sockelist的内容采集到sysStateTable,用于显示*/
void Table::SysStatusTableUpdate()
{
    int counter;
    //迭代器
    QList <BoxItem>::iterator itSOK;
    QList <DATA_SYS_ITEM>::iterator itItm;
    QList <DATA_SYS_DATA>::iterator itDat;
    QList <uint8_t>::iterator itBID;
    QList <DATA_BINGQU>::iterator itBTable;
    //临时变量
    DATA_SYS_ITEM tmpItem;
    DATA_SYS_DATA tmpData;

    //首先根据此刻有多少连接，决定
    if(boxList.size() == 0)
    {
        sysStateTable.clear();
        return;
    }

    //先清零，准备填入数据
    sysStateTable.clear();

    for(itSOK = boxList.begin(); itSOK != boxList.end(); itSOK++)
    {

        //清空
        tmpData.boxId = "";
        tmpData.taskText = "";

        //先填入转运箱ID
        //然后填入任务列表 病区ID 病区名称
        //填入机器人所在病区
        if(itSOK->positionStatus == 0)
            tmpData.boxId = itSOK->boxID + "初始化";
        else
            tmpData.boxId = itSOK->boxID + "正常";

        //如果任务不为零
        if(itSOK->expressList->size() != 0)
        {
            for(counter = 0; counter < itSOK->expressList->size(); counter++)
            {
                //获得任务中的病区ID
                //tmpItem.bingquId = QString::number(itSOK->expressList->at(counter));
                //查表更新任务中的病区名称...
                if(itSOK->expressList->at(counter).alreadyReached == 0)
                {
                    tmpData.taskText += itSOK->expressList->at(counter).name;
                    tmpData.taskText += "->";
                }
                if(itSOK->expressList->at(counter).alreadyReached == 1)
                {
                    tmpData.taskText += "【" + itSOK->expressList->at(counter).name + "】";
                    tmpData.taskText += "->";
                }
                if(itSOK->expressList->at(counter).alreadyReached == 2)
                {
                    tmpData.taskText += "（" + itSOK->expressList->at(counter).name + "）";
                    tmpData.taskText += "->";
                }

            }
            //添加到sysStateTable

        }
        else
        {
            //不作任何处理 不写入任何任务信息
        }

        sysStateTable.append(tmpData);

    }
}
void Table::CallMirStatusUpdate()
{
    ResetCalledMir();
}
/*XiangtiTable的配置更新到boxlist的相关项，确保连接有意义*/
void Table::BoxListUpdate()
{
    //遍历socklist的id,查询是否在箱体配置表中有这一项
    //如果没有，就清空此socket连接的mir数据
    //如果有，就更新一下mir数据
    BoxItem AddBoxItem;

    QList <BoxItem>::iterator itBox;
    QList <DATA_XIANGTI>::iterator itXiangti;
    int counter;

    /*个数保持一致*/
    if(boxList.size() < xiangtiTable.size())
    {
        counter = xiangtiTable.size() - boxList.size();
        for(int i = 0; i < counter; i++)
        {
            AddBoxItem.boxID = "0"; //实验
            //clientItem.boxID = "";
            AddBoxItem.boxStatus = 0;
            AddBoxItem.pos_x = 0;
            AddBoxItem.pos_y = 0;
            AddBoxItem.MIRStatus = 0;
            AddBoxItem.missionState = "waiting";
            AddBoxItem.missionStateChange = 0;
            AddBoxItem.execCounter = 0;
            AddBoxItem.positionStatus = 0;
            /*初始默认位置，后续加入动态查询*/
            AddBoxItem.targetBingqu = "静配中心";
            AddBoxItem.targetBingquId = 0;
            AddBoxItem.targetBingquRange = 0;

            /*这里有必要先检查一下QList中是否存在相同的handle*/

            /*容器一定要初始化为指定指针值(实例化对象)，注意释放*/
            /*目前只含有这些list，就一级*/
            AddBoxItem.expressList = new QList <ExpressItem>{};
            AddBoxItem.missionGuidList = new QList <MISSION_INFO>{};
            AddBoxItem.positonGuidList = new QList <POSITION_INFO>{};
            AddBoxItem.casheGuidList = new QList <POSITION_INFO>{};

            boxList.append(AddBoxItem);
        }
    }
    else if(boxList.size() > xiangtiTable.size())
    {
        //一项一项删除
        boxList.last().expressList->clear();
        boxList.last().missionGuidList->clear();
        boxList.last().positonGuidList->clear();
        boxList.last().casheGuidList->clear();
        delete boxList.last().expressList;
        delete boxList.last().missionGuidList;
        delete boxList.last().positonGuidList;
        delete boxList.last().casheGuidList;

        boxList.removeLast();
    }

    /*数目相等之后，一项一项更新id url*/
    if(boxList.size() == xiangtiTable.size())
    {
        for(itBox = boxList.begin(), itXiangti = xiangtiTable.begin();
            itBox != boxList.end()||itXiangti != xiangtiTable.end() ;
            itBox++,itXiangti++)
        {

            itBox->boxID = itXiangti->id;
            itBox->url = "http://"+ itXiangti->ip + "/api/v2.0.0/" ;
            itBox->port = itXiangti->port;
        }
    }
}

/*socketlist中箱体一旦有任务了 (只操作socketList表)
根据missionText得到missionState和目标病区以及变化的标志位
这是周期运行的

httpRequest请求类型 1:下任务并执行 2:叫车 */
void Table::SocketTaskProcess()
{
    QList <BoxItem>::iterator it;
    int counter,findItFlag;
    QByteArray temp;
    uint8_t targetBingquId;
    QString targetBingquName;

    if(boxList.size() == 0)
        return;

    //遍历所有连接，更新missionNum和机器人的状态
    for(it = boxList.begin(); it != boxList.end(); it++)
    {
        it->missionNum = it->expressList->size();

        //当前状态和之前状态做对比，得到启动和停止的信号
        if(it->missionState == "waiting")
        {
            //当前状态和之前状态进行比较
            if(it->previousMissionState != "")
            {
                if(it->previousMissionState == "working")
                {
                    /*working -> waiting*/
                    it->missionStateChange = 1;
                }
                if(it->previousMissionState == "waiting")
                {
                    /*waiting*/
                   // it->missionStateChange = 0;
                }
            }
            else if(it->previousMissionState == "")
            {
                it->previousMissionState = "waiting";
            }

            it->previousMissionState = "waiting";
        }
        else if(it->missionState == "working")
        {
            //当前状态和之前状态进行比较
            if(it->previousMissionState != "")
            {
                if(it->previousMissionState == "waiting")
                {
                     /*waiting -> working*/
                    it->missionStateChange = 2;
                }
                if(it->previousMissionState == "working")
                {
                    /*working*/
                   // it->missionStateChange = 0;
                }
            }
            else if(it->previousMissionState == "")
            {
                it->previousMissionState = "working";
            }

            it->previousMissionState = "working";
        }
    }

    //遍历所有连接，执行叫车服务
    if(callForMir == 1)     //出现了叫车服务
    {
        findItFlag = 0;
        for(it = boxList.begin(); it != boxList.end(); it++)
        {
            //没有任务 电量充足 当前状态ready
            if(it->expressList->size() == 0)
            {
                findItFlag = 1; //找到了
                //相应socket的请求标志置位
                it->httpRequest = 2;
                //更改系统的相关数据
                callForMir = 0; //清空需求
                qDebug()<<it->boxID<<"被请求";
                calledMir = it->boxID;

                break;
            }
        }
        //如果没有找到，callforMir会一直存在
    }

}
/*此函数执行socket列表中的任务，一旦有任务，就根据箱体配置表中的数据，找到对应的机器人信息表，之后一个post missionqueue操作就可以了*/
void Table::RxBuff2TcpBuff()
{
    int socketDesciptor;
    QByteArray dataByteArray;//通用的处理array,存下来
    QList<TcpItem>::iterator it;

    /*前提判断*/
    if(GetRxBuffer()->size() == 0)
        return;
    if(socketList.size() == 0)
    {
        GetRxBuffer()->clear(); /*没有连接就清空缓冲*/
        return;
    }

    /*读取总缓冲区，遍历填入到TcpBuff*/
    dataByteArray = GetRxBuffer()->at(0).rxData;
    socketDesciptor = GetRxBuffer()->at(0).socketDesciptor;
    GetRxBuffer()->removeAt(0);             //读出来后删掉缓冲区中的此项

    //qDebug()<<"raw"<<dataByteArray;

    /*确定Tcp, 并且接上socketlist的相应Qbytearray中*/
    for(it = socketList.begin() ; it != socketList.end() ; it++)
    {
        if(socketDesciptor == it->socketDesciptor)
        {
            it->rawInputData += dataByteArray;  /*接上去*/
            qDebug()<<it->rawInputData;
        }
    }
}
void Table::TcpBuffVertify()
{
    uint16_t length;
    QByteArray dataByteArray;//通用的处理array,存下来
    QList<TcpItem>::iterator it;

    /*前提判断*/
    if(socketList.size() == 0)
    {
        GetRxBuffer()->clear(); /*没有连接就清空缓冲*/
        GetTxBuffer()->clear();
        return;
    }

    /*遍历TcpBuff*/
    for(it = socketList.begin() ; it != socketList.end() ; it++)
    {
        if(it->rawInputData.size() > 0) //有数据
        {
            if(it->rawInputData[0] == 0x55) //有必要进行判断
            {
                if(it->rawInputData[1] == 0xAA) //帧头有效
                {
                    qDebug()<<"帧头有效";
                    if(it->rawInputData.size() > 5) //基本帧头存在
                    {
                        length = (it->rawInputData[5] << 8) | it->rawInputData[4];
                        qDebug()<<"长度"<<QString::number(length);
                        if(length > 128)
                        {
                            //错误！删除这个假的帧头
                            it->rawInputData.remove(0,2);//删除他们两个
                            //it->rawInputData.clear();
                        }
                        if((length + 8) <= it->rawInputData.size()) //帧头6字节
                        {
                            qDebug()<<it->rawInputData;
                            //数据完整了,截取出来
                            dataByteArray.resize(length+8);
                            memcpy(dataByteArray.data(),it->rawInputData.data(),length+8);
                            it->rawInputData.remove(0,length+8);
                            qDebug()<<it->rawInputData;
                            //开始解析，由于数据帧包含箱体编号，因此通用
                            TcpFrameProcess(it->socketDesciptor,dataByteArray);
                        }
                        else
                        {
                            //不处理，还没有接受完全
                        }
                    }
                    else                            //基本帧头不存在
                    {
                        //不处理，还没有接受完全
                    }

                }
                else                            //帧头无效
                {
                    it->rawInputData.remove(0,2);//删除他们两个
                }
            }
            else                            //没必要进行判断了
            {
                it->rawInputData.remove(0,1); //左移1字节
            }
        }
    }
    /*寻找每一个TcpBuff的第一项和第二项 0xaa 0x55*/
    /*每找到，就不停前移，直到找到或者没有*/
    /*如果找到，计算length+8是否完整*/
    /*如果不完整，不处理等待*/
    /*如果完整，提取出来，直接TcpFrameProcess()*/
}
/*哪个tcp通道，的完整数据，接下来验证crc*/
void Table::TcpFrameProcess(int socketDesciptor, QByteArray dataByteArray)
{
    REQUEST formatData;

    int size,i,j,findItFlag;
    uint utcCount;
    static uint8_t bingquItemNum = 3;   /*需要长期保存*/
    static uint8_t yihuItemNum = 3;
    uint8_t bingquStartNum,yihuStartNum;
    uint8_t actualItemNum;
    uint8_t temp;
    uint16_t crcValue;
    uint8_t group,groupNum;

    TX_DATA_ITEM txDataBuffList;//用于插入发送缓冲的通用项
    RX_DATA_ITEM rxDataBuffList;//用于处理接收缓冲的通用项
    BoxItem tcpDemoItem;        //用于处理Qlist的查询
    QList <TcpItem>::iterator itSok;
    QList <BoxItem>::iterator it;//用于处理Qlist的修改
    QList <ExpressItem>::iterator itExpress;//任务迭代器
    //QList <DATA_YIHU> * vDataYihu = nullptr;
    QVector<YihuData> vDataYihu;

    /*mirschedule*/
    MIRSchedule * mirSchedule = nullptr;


    DATA_ACK dataAck;       //用于回复病区和医护的头
    ExpressItem dataTask;
    DATA_HEARTBEAT_REQUEST_ACK dataHeart;

    //验证crc 暂时先不进行验证
    qDebug()<<dataByteArray.length();

    crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
    if(dataByteArray[dataByteArray.size()-2] != (crcValue & 0xff) ||
        dataByteArray[dataByteArray.size()-1] != (crcValue >> 8) )
    {
        qDebug()<<"crc校验错误 丢弃";
        return;
    }

    //首先复制到buff，转化为指定的struct数据帧格式
    memcpy(formatData.buff, dataByteArray.data(), dataByteArray.size());

    //然后解析cmd
    switch (formatData.generalData.cmd)
    {
    case 0xA11A: //病区请求 boxid无关(不用动)

        /*先确定哪一组，每组数量*/
        group = formatData.generalData.group;
        groupNum = formatData.generalData.groupNum;
        size = bingquTable.size();

        qDebug()<<"length"<<formatData.generalData.length;
        qDebug()<<"group"<<formatData.generalData.group;
        qDebug()<<"groupNum"<<formatData.generalData.groupNum;
        qDebug()<<"crc"<<formatData.generalData.crc16;
        qDebug()<<"size"<<size;
        /*如果group=1,就确定了每组数量m，每组数据就定下来了*/
        if(group == 1)
            bingquItemNum = groupNum;
        qDebug()<<"group"<<bingquItemNum;
        /*获得病区表全部数据*/
        dataAck;

        /*groupNum 12345*/
        /*首先确定此group 1234 size 01234下有多少项目*/

        /*根据组号首先确定每一组可用的实际项目数，不会超过这个数*/
        if(group <= size/bingquItemNum && group > 0) //完整帧
        {
            actualItemNum = bingquItemNum;
            if(groupNum < actualItemNum)
                actualItemNum = groupNum;
            bingquStartNum = bingquItemNum * (group - 1);
            qDebug()<<"wanzheng"<<actualItemNum<<";"<<"start"<<bingquStartNum;
        }
        else                                         //不完整帧
        {
            actualItemNum = size%bingquItemNum; //计算得到的余量
            if(groupNum < actualItemNum)
                actualItemNum = groupNum;
            bingquStartNum = groupNum * (size/bingquItemNum);
            qDebug()<<"yuliang"<<actualItemNum<<";"<<"start"<<bingquStartNum;
        }
        /*以上 根据组号确定从哪一项开始*/

        /*构造帧头，填入长度，填入数据*/
        dataAck.header = formatData.generalData.header;
        dataAck.cmd = formatData.generalData.cmd;
        dataAck.length = (18 * actualItemNum);

        dataByteArray.resize(18 * actualItemNum + 8);

        memcpy(dataByteArray.data(),&dataAck,6);    //直接小端复制
        //dataByteArray[0] = dataAck.header >> 8;
        //dataByteArray[1] = dataAck.header;
        //dataByteArray[2] = dataAck.cmd >> 8;
        //dataByteArray[3] = dataAck.cmd;
        //dataByteArray[4] = (18 * actualItemNum) >> 8;
        //dataByteArray[5] = (18 * actualItemNum);
        /*从6开始，总共加上以下项*/
        for(i = 0, j = 6; i < actualItemNum; i++ , j += 18) //j负责实际帧 i负责病区表的项数
        {
            dataByteArray[j] = bingquTable.at(bingquStartNum + i).id;
            dataByteArray[j + 1] = (bingquTable.at(bingquStartNum + i).id) >> 8;
            for(int temp = 0; temp < 16; temp++)
            {
                dataByteArray[j + 2 + temp] = bingquTable.at(bingquStartNum + i).name[temp];
            }
        }

        //CRC校验
        crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
        dataByteArray[dataByteArray.size()-2] = crcValue;
        dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

        /*插入缓冲区*/
        txDataBuffList.socketDesciptor = socketDesciptor;
        txDataBuffList.txData = dataByteArray;
        /*定位要发送的全部数据*/

        GetTxBuffer()->append(txDataBuffList);
        break;

    case 0xB22B: //医护请求 boxid无关 （不用动）
        /*先确定哪一组，每组数量*/
        group = formatData.generalData.group;
        groupNum = formatData.generalData.groupNum;
        size = yihuTable.size();

        qDebug()<<"length"<<formatData.generalData.length;
        qDebug()<<"group"<<formatData.generalData.group;
        qDebug()<<"groupNum"<<formatData.generalData.groupNum;
        qDebug()<<"crc"<<formatData.generalData.crc16;
        qDebug()<<"size"<<size;
        /*如果group=1,就确定了每组数量m，每组数据就定下来了*/
        if(group == 1)
            yihuItemNum = groupNum;
        qDebug()<<"group"<<yihuItemNum;
        /*获得病区表全部数据*/
        dataAck;

        /*groupNum 12345*/
        /*首先确定此group 1234 size 01234下有多少项目*/

        /*根据组号首先确定每一组可用的实际项目数，不会超过这个数*/
        if(group <= size/yihuItemNum && group > 0) //完整帧
        {
            actualItemNum = yihuItemNum;
            if(groupNum < actualItemNum)
                actualItemNum = groupNum;
            yihuStartNum = yihuItemNum * (group - 1);
            qDebug()<<"wanzheng"<<actualItemNum<<";"<<"start"<<yihuStartNum;
        }
        else                                         //不完整帧
        {
            actualItemNum = size%yihuItemNum; //计算得到的余量
            if(groupNum < actualItemNum)
                actualItemNum = groupNum;
            yihuStartNum = groupNum * (size/yihuItemNum);
            qDebug()<<"yuliang"<<actualItemNum<<";"<<"start"<<yihuStartNum;
        }
        /*以上 根据组号确定从哪一项开始*/

        /*构造帧头，填入长度，填入数据*/
        dataAck.header = formatData.generalData.header;
        dataAck.cmd = formatData.generalData.cmd;
        dataAck.length = (actualItemNum)*12;

        dataByteArray.resize(12 * actualItemNum + 8);
        dataByteArray[0] = dataAck.header >> 8;
        dataByteArray[1] = dataAck.header;
        dataByteArray[2] = dataAck.cmd >> 8;
        dataByteArray[3] = dataAck.cmd;
        dataByteArray[4] = (12 * actualItemNum) >> 8;
        dataByteArray[5] = (12 * actualItemNum);
        /*从6开始，总共加上以下项*/
        for(i = 0, j = 6; i < actualItemNum; i++ , j += 12) //j负责实际帧 i负责医护表的项数
        {
            dataByteArray[j] = yihuTable.at(yihuStartNum + i).id >> 8;
            dataByteArray[j+ 1] = yihuTable.at(yihuStartNum + i).id;
            for(int temp = 0; temp < 8; temp++)
            {
                dataByteArray[j+temp+2] = yihuTable.at(yihuStartNum + i).name[temp];
            }
            dataByteArray[j + 10] = (yihuTable.at(yihuStartNum + i).bingquId) >> 8;
            dataByteArray[j + 11] = yihuTable.at(yihuStartNum + i).bingquId;
        }

        //CRC校验
        crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
        dataByteArray[dataByteArray.size()-2] = crcValue >> 8;
        dataByteArray[dataByteArray.size()-1] = crcValue;

        /*插入缓冲区*/
        txDataBuffList.socketDesciptor = socketDesciptor;
        txDataBuffList.txData = dataByteArray;
        /*定位要发送的全部数据*/

        GetTxBuffer()->append(txDataBuffList);
        break;

    case 0xD44D:    //运单任务数据 boxid相关

        /*接收到以后填入任务表*/ 
        for(it = boxList.begin() ; it != boxList.end() ; it++)
        {
            if(formatData.expressData.boxId == it->boxID.toInt())
            {
                /*接收到的话，清零原来的任务列表*/
                it->expressList->clear();

                //如果说没有病区的情况
                if(formatData.expressData.length >= 4)
                {
                    for(j = 0; j < (formatData.expressData.length - 2); j = j + 2)
                    {
                        //dataTask.taskBingquId = formatData.expressData.bingqu[j];
                        dataTask.taskBingquId = (dataByteArray[9 + j] << 8) | dataByteArray[8 + j];
                        dataTask.alreadyReached = 0;
                        dataTask.distance = 0.0;
                        dataTask.guid = "";
                        dataTask.map = "";
                        dataTask.name = "";
                        dataTask.range = 0;
                        dataTask.url = "";
                        dataTask.X = 0.0;
                        dataTask.Y = 0.0;
                        dataTask.timeout = 0;
                        it->expressList->append(dataTask);
                        qDebug()<<"请求的病区id"<<dataTask.taskBingquId;
                    }
                    /*最后加上集散地*/
                    dataTask.taskBingquId = 0xffff; //约定
                    dataTask.alreadyReached = 0;
                    dataTask.distance = 0.0;
                    dataTask.guid = "";
                    dataTask.map = "";
                    dataTask.name = "集散地";      //不用查表，直接赋值
                    dataTask.range = 0;
                    dataTask.url = "";
                    dataTask.X = 0.0;
                    dataTask.Y = 0.0;
                    dataTask.timeout = 0;
                    it->expressList->append(dataTask);
                }
                else
                {
                    //不含数据或者异常数据
                }


                //额外的处理 新的任务列表的初始状态 但是没有任务也需要地址 所以还是注释掉吧
                /*跳出遍历，必须只有一个满足*/
                break;
            }
        }
        txDataBuffList.socketDesciptor = socketDesciptor;
        txDataBuffList.txData = dataByteArray;
        GetTxBuffer()->append(txDataBuffList);

        /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
        break;

    case 0xE55E:    //机器人位置信息 boxId相关

        /*构造帧头，填入长度，填入数据*/
        dataAck.header = formatData.generalData.header;
        dataAck.cmd = formatData.generalData.cmd;

        for(itSok = socketList.begin() ; itSok != socketList.end() ; itSok++)
        {
            if(socketDesciptor == itSok->socketDesciptor)
            {
                /*utc更新*/
                itSok->utc = QDateTime::currentDateTime().toTime_t();
            }
        }

        /*遍历所有连接 定位boxId*/
        /*如果boxId存在*/
        if(RMSFactory()->boxMap.contains(formatData.startCmdData.boxId))
        {
            if(RMSFactory()->boxMap.value(formatData.startCmdData.boxId))
            {
                mirSchedule = RMSFactory()->boxMap.value(formatData.startCmdData.boxId)->mirSchedule;
                int bingquId = bingquTableCON->GetBingquId(mirSchedule->state.MIRTargetBingqu);

                /*构造帧头，填入长度，填入数据*/
                dataByteArray.resize(14);
                dataByteArray[0] = dataAck.header;
                dataByteArray[1] = dataAck.header >> 8;
                dataByteArray[2] = dataAck.cmd;
                dataByteArray[3] = dataAck.cmd >> 8;
                dataByteArray[4] = 6;
                dataByteArray[5] = 0;
                dataByteArray[6] = formatData.MIRData.boxId;
                dataByteArray[7] = formatData.MIRData.boxId >> 8;
                //找到机器人的任务列表 如果不为0
                //在任务列表中找 即将到达的病区
                //如果没有，就找最后一个已经到达的病区

                dataByteArray[8] = mirSchedule->state.MIRState;
                dataByteArray[9] = mirSchedule->state.MIRState >> 8;
                dataByteArray[10] = bingquId;
                dataByteArray[11] = bingquId >> 8;


                /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                dataByteArray[dataByteArray.size()-2] = crcValue;
                dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                txDataBuffList.socketDesciptor = socketDesciptor;
                txDataBuffList.txData = dataByteArray;
                GetTxBuffer()->append(txDataBuffList);
            }
            else
            {
                //根本没有找到mirSchedule
                dataByteArray.resize(14);
                dataByteArray[0] = dataAck.header;
                dataByteArray[1] = dataAck.header >> 8;
                dataByteArray[2] = dataAck.cmd;
                dataByteArray[3] = dataAck.cmd >> 8;
                dataByteArray[4] = 6;
                dataByteArray[5] = 0;
                dataByteArray[6] = formatData.MIRData.boxId;
                dataByteArray[7] = formatData.MIRData.boxId >> 8;
                dataByteArray[8] = 0;
                dataByteArray[9] = 0;
                dataByteArray[10] = 0;
                dataByteArray[11] = 0;

                /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                dataByteArray[dataByteArray.size()-2] = crcValue;
                dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                txDataBuffList.socketDesciptor = socketDesciptor;
                txDataBuffList.txData = dataByteArray;
                GetTxBuffer()->append(txDataBuffList);
            }
        }
        break;

    case 0xF66F:    //心跳数据

        /*接收到以后填入任务表*/
        /*首先找到相同socketDesciptor的项*/
        if(socketList.size() == 0)
            break;

        //for(it = socketList.begin(); it !=  socketList.end(); it++)
        {
            //qDebug()<<socketList->at(i).boxID ;

            //首先确定连接通道，发送接收缓冲用的
            //if(socketDesciptor == it->socketDesciptor)
            {
                /*如果boxid相等就不处理了，不等再处理*/
                it->boxID = QString::number(formatData.heartbeatData.boxId);

                /*加上UTC。。*/
                *dateTime = QDateTime::currentDateTime();
                utcCount = dateTime->toTime_t();

                /*构造帧头，填入长度，填入数据*/
                dataAck.header = formatData.generalData.header;
                dataAck.cmd = formatData.generalData.cmd;
                dataAck.length = 8;

                dataByteArray.resize(16);
                dataByteArray[0] = dataAck.header;
                dataByteArray[1] = dataAck.header >> 8;
                dataByteArray[2] = dataAck.cmd;
                dataByteArray[3] = dataAck.cmd >> 8;
                dataByteArray[4] = 8;
                dataByteArray[5] = 0;
                dataByteArray[6] = formatData.heartbeatData.boxId;
                dataByteArray[7] = formatData.heartbeatData.boxId >> 8;
                dataByteArray[8] = utcCount;
                dataByteArray[9] = utcCount >> 8;
                dataByteArray[10] = utcCount >> 16;
                dataByteArray[11] = utcCount >> 24;
                dataByteArray[12] = 0;
                dataByteArray[13] = 0;

                /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                dataByteArray[dataByteArray.size()-2] = crcValue >> 8;
                dataByteArray[dataByteArray.size()-1] = crcValue;

                txDataBuffList.socketDesciptor = socketDesciptor;
                txDataBuffList.txData = dataByteArray;
                GetTxBuffer()->append(txDataBuffList);

                /*跳出遍历，必须只有一个满足*/
                break;
            }
        }
        /*原样返回*/
        break;

    case 0x9779:    //密码数据
        /*容错判断*/
        crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
        if(formatData.passwordData.crc16 != crcValue)
            break;
        if(formatData.passwordData.length != 50)
            break;
        /*接收到以后填入任务表*/
        /*首先找到相同socketDesciptor的项*/
        if(socketList.size() == 0)
            break;

        //for(it = socketList.begin(); it !=  socketList.end(); it++)
        {
            //首先由socketDesciptor确定连接通道，发送接收缓冲用的
            //if(socketDesciptor == it->socketDesciptor)
            {

                QByteArray manaArray;
                manaArray.resize(16);//重置数据大小
                memcpy(manaArray.data(), formatData.passwordData.managePassword, 16);//copy数据
                QByteArray opArray;
                opArray.resize(16);//重置数据大小
                memcpy(opArray.data(), formatData.passwordData.operatePassword, 16);//copy数据
                QByteArray openArray;
                openArray.resize(16);//重置数据大小
                memcpy(openArray.data(), formatData.passwordData.openBoxPassword, 16);//copy数据

                //先变成QString
                QString manaStr(manaArray);
                QString opStr(opArray);
                QString openStr(openArray);

                if(manaStr == manaCode &&
                        opStr == opCode &&
                        openStr == openCode)
                {
                    dataAck.header = formatData.passwordData.header;
                    dataAck.cmd = formatData.passwordData.cmd;

                    dataByteArray.resize(58);       /*header 4 + length 2 + id 2 + password 48 + crc 2*/
                    dataByteArray[0] = dataAck.header >> 8;
                    dataByteArray[1] = dataAck.header;
                    dataByteArray[2] = 0x79;
                    dataByteArray[3] = 0x97;
                    dataByteArray[4] = 50;
                    dataByteArray[5] = 0;
                    dataByteArray[6] = formatData.heartbeatData.boxId;
                    dataByteArray[7] = formatData.heartbeatData.boxId >> 8;

                    memset(dataByteArray.data()+8,0,16);
                    memcpy(dataByteArray.data()+8, manaCode.toLatin1().data(), manaCode.length());//copy数据
                    memset(dataByteArray.data()+24,0,16);
                    memcpy(dataByteArray.data()+24, opCode.toLatin1().data(), opCode.length());//copy数据
                    memset(dataByteArray.data()+40,0,16);
                    memcpy(dataByteArray.data()+40, openCode.toLatin1().data(), openCode.length());//copy数据
                }
                else
                {
                    dataAck.header = formatData.passwordData.header;
                    dataAck.cmd = formatData.passwordData.cmd;

                    dataByteArray.resize(58);   /*header 4 + length 2 + id 2 + password 48 + crc 2*/
                    dataByteArray[0] = dataAck.header >> 8;
                    dataByteArray[1] = dataAck.header;
                    dataByteArray[2] = 0x78;
                    dataByteArray[3] = 0x87;
                    dataByteArray[4] = 50;
                    dataByteArray[5] = 0;
                    dataByteArray[6] = formatData.heartbeatData.boxId;
                    dataByteArray[7] = formatData.heartbeatData.boxId >> 8;

                    memset(dataByteArray.data()+8,0,16);
                    memcpy(dataByteArray.data()+8, manaCode.toLatin1().data(), manaCode.length());//copy数据
                    memset(dataByteArray.data()+24,0,16);
                    memcpy(dataByteArray.data()+24, opCode.toLatin1().data(), opCode.length());//copy数据
                    memset(dataByteArray.data()+40,0,16);
                    memcpy(dataByteArray.data()+40, openCode.toLatin1().data(), openCode.length());//copy数据
                }


                /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                dataByteArray[dataByteArray.size()-2] = crcValue;
                dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                txDataBuffList.socketDesciptor = socketDesciptor;
                txDataBuffList.txData = dataByteArray;
                GetTxBuffer()->append(txDataBuffList);

                /*跳出遍历，必须只有一个满足*/
                break;
            }
        }
        /*原样返回*/
        break;

    case 0xC33C:    //指纹下载

        /*容错判断*/
        if(formatData.fingerRequestData.length != 5)
            break;

        /*首先找到相同socketDesciptor的项*/
        if(socketList.size() == 0)
            break;

        /*构造帧头，填入长度，填入数据*/
        dataAck.header = formatData.generalData.header;
        dataAck.cmd = formatData.generalData.cmd;

        /*获得医护数据*/
        vDataYihu = yihuTableCON->GetYihuResultWithFinger();

        for(itSok = socketList.begin(); itSok !=  socketList.end(); itSok++)
        {
            /*首先由socketDesciptor确定连接通道，发送接收缓冲用的*/
            if(socketDesciptor == itSok->socketDesciptor)
            {
                //通过员工表得到->某病区的工号+指纹
                if(!vDataYihu.isEmpty())
                {
                    findItFlag = 0; /*查找标志*/
                    for(int i = 0; i < vDataYihu.size(); i++)
                    {
                        /*按照某一个固定的病区来进行查找，当然肯定有找不到的情况*/
                        if(vDataYihu.at(i).bingquId == formatData.fingerRequestData.bingquId)
                        {
                            findItFlag = 1;

                            bool ret;
                            QByteArray tempBa;
                            QString data = vDataYihu.at(i).strFinger;
                            QStringList result = data.split(QRegExp("[ ]"));
                            foreach (QString item, result)
                            {
                                tempBa.append(item.toInt(&ret,16));
                            }

                            for(int group = 0; tempBa.size() != 0; group++)
                            {
                                if(tempBa.size() == 0 || tempBa.size() < 128)
                                {
                                    dataAck.length = tempBa.size() + 5; //病2 工2 组1 (不包含CRC 2)
                                    dataByteArray.resize(dataAck.length + 8);       //帧头+crc 8
                                    dataByteArray[0] = dataAck.header;
                                    dataByteArray[1] = dataAck.header >> 8;
                                    dataByteArray[2] = dataAck.cmd;
                                    dataByteArray[3] = dataAck.cmd >> 8;
                                    dataByteArray[4] = dataAck.length;
                                    dataByteArray[5] = dataAck.length >> 8;
                                    dataByteArray[6] = formatData.fingerRequestData.bingquId;
                                    dataByteArray[7] = formatData.fingerRequestData.bingquId >> 8;
                                    dataByteArray[8] = vDataYihu.at(i).id;
                                    dataByteArray[9] = vDataYihu.at(i).id >> 8;
                                    dataByteArray[10] = group;
                                    //以下从dataByteArray[11]拷贝指纹数据
                                    memcpy(dataByteArray.data() + 11, tempBa.data(), tempBa.size());
                                    tempBa.remove(0,tempBa.size());
                                }
                                else
                                {
                                    dataAck.length = 128 + 5;   //病2 工2 组1 (不包含CRC 2)
                                    dataByteArray.resize(dataAck.length + 8);   //帧头+crc 8
                                    dataByteArray[0] = dataAck.header;
                                    dataByteArray[1] = dataAck.header >> 8;
                                    dataByteArray[2] = dataAck.cmd;
                                    dataByteArray[3] = dataAck.cmd >> 8;
                                    dataByteArray[4] = dataAck.length;
                                    dataByteArray[5] = dataAck.length >> 8;
                                    dataByteArray[6] = formatData.fingerRequestData.bingquId;
                                    dataByteArray[7] = formatData.fingerRequestData.bingquId >> 8;
                                    dataByteArray[8] = vDataYihu.at(i).id;
                                    dataByteArray[9] = vDataYihu.at(i).id >> 8;
                                    dataByteArray[10] = group;
                                    //以下从dataByteArray[11]拷贝指纹数据
                                    memcpy(dataByteArray.data() + 11, tempBa.data(), 128);
                                    tempBa.remove(0,128);
                                }

                                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                                dataByteArray[dataByteArray.size()-2] = crcValue;
                                dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                                txDataBuffList.socketDesciptor = socketDesciptor;
                                txDataBuffList.txData = dataByteArray;
                                GetTxBuffer()->append(txDataBuffList);
                            }
                        } 
                    }
                    if(findItFlag == 0) /*没有找到对应此病区的指纹*/
                    {
                        dataAck.length = 5;
                        dataByteArray.resize(dataAck.length + 8);   //帧头+crc 8
                        dataByteArray[0] = dataAck.header;
                        dataByteArray[1] = dataAck.header >> 8;
                        dataByteArray[2] = dataAck.cmd;
                        dataByteArray[3] = dataAck.cmd >> 8;
                        dataByteArray[4] = dataAck.length;
                        dataByteArray[5] = dataAck.length >> 8;
                        dataByteArray[6] = formatData.fingerRequestData.bingquId;
                        dataByteArray[7] = formatData.fingerRequestData.bingquId >> 8;
                        dataByteArray[8] = 0;   /*工号 0 无效 表示没有*/
                        dataByteArray[9] = 0;
                        dataByteArray[10] = 0;  /*组号 0 无效 表示没有*/

                        crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                        dataByteArray[dataByteArray.size()-2] = crcValue;
                        dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                        txDataBuffList.socketDesciptor = socketDesciptor;
                        txDataBuffList.txData = dataByteArray;
                        GetTxBuffer()->append(txDataBuffList);
                    }
                }
                else    /*压根就没有指纹，也要回复*/
                {
                    dataAck.length = 5;
                    dataByteArray.resize(dataAck.length + 8);   //帧头+crc 8
                    dataByteArray[0] = dataAck.header;
                    dataByteArray[1] = dataAck.header >> 8;
                    dataByteArray[2] = dataAck.cmd;
                    dataByteArray[3] = dataAck.cmd >> 8;
                    dataByteArray[4] = dataAck.length;
                    dataByteArray[5] = dataAck.length >> 8;
                    dataByteArray[6] = formatData.fingerRequestData.bingquId;
                    dataByteArray[7] = formatData.fingerRequestData.bingquId >> 8;
                    dataByteArray[8] = 0;   /*工号 0 无效 表示没有*/
                    dataByteArray[9] = 0;
                    dataByteArray[10] = 0;  /*组号 0 无效 表示没有*/

                    crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                    dataByteArray[dataByteArray.size()-2] = crcValue;
                    dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

                    txDataBuffList.socketDesciptor = socketDesciptor;
                    txDataBuffList.txData = dataByteArray;
                    GetTxBuffer()->append(txDataBuffList);
                }
            }
        }
        break;

    case 0xC55C:    //指纹上传
        /*前大小端转换,接收的数据正常化*/
        formatData.fingerReportData.length = qFromBigEndian(formatData.fingerReportData.length);
        formatData.fingerReportData.crc16 = qFromBigEndian(formatData.fingerReportData.crc16);

        /*容错判断*/
        if(formatData.fingerReportData.length != (dataByteArray.size() - 6))
            break;

        /*首先找到相同socketDesciptor的项*/
        if(socketList.size() == 0)
            break;

        //for(it = socketList.begin(); it !=  socketList.end(); it++)
        {
            //首先由socketDesciptor确定连接通道，发送接收缓冲用的
            //if(socketDesciptor == it->socketDesciptor)
            {
                //将病区 工号 组号 数据 压入到list中
                //周期性地扫描他们，如果其中相同病区 工号的项目中
                //组好满足1-n连续 且 n的数据长度不是128B 那么合并
                //这个数据缓冲是所有socket共享的，table中。

                /* 先构造一个缓冲区单项，然后插入发送缓冲区*/
                crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
                dataByteArray[dataByteArray.size()-2] = crcValue >> 8;
                dataByteArray[dataByteArray.size()-1] = crcValue;

                txDataBuffList.socketDesciptor = socketDesciptor;
                txDataBuffList.txData = dataByteArray;
                GetTxBuffer()->append(txDataBuffList);

                /*跳出遍历，必须只有一个满足*/
                break;
            }
        }
        break;

    case 0xD66D:    //发车指令

        /*遍历所有箱体,根据Boxid锁定boxMap箱体boxUnit*/
        findItFlag = 0;
        if(RMSFactory()->boxMap.contains(formatData.startCmdData.boxId))
        {
            RMSFactory()->boxMap.value(formatData.startCmdData.boxId)->SetGoCmd(1);
        }

        /*原样返回 先构造一个缓冲区单项，然后插入发送缓冲区*/
        crcValue = Get_Crc16((uint8_t *)(dataByteArray.data()),dataByteArray.size()-2);
        dataByteArray[dataByteArray.size()-2] = crcValue;
        dataByteArray[dataByteArray.size()-1] = crcValue >> 8;

        txDataBuffList.socketDesciptor = socketDesciptor;
        txDataBuffList.txData = dataByteArray;
        GetTxBuffer()->append(txDataBuffList);

        break;

    default:
        break;
    }
}

/*table负责检查有没有数据帧可以组成zhi*/
void Table::FingerDataCheck()
{
    QSqlRecord record = fingerModule->model->record();

    QList <FINGER_DATA_ITEM>::iterator it;
    QList <FINGER_DATA_ITEM>::iterator it1;
    QList <FINGER_DATA_ITEM> * pFingerBuff;
    uint8_t lastGroup,findItFlag,counter,okFlag;
    uint8_t bingquId,yihuId;        //暂存病区 医护 最大组号

    QByteArray totalData;

    pFingerBuff = GetFingerBuffer();
    if(pFingerBuff->isEmpty())
        return;

    //首先记录每个项目再缓冲区中的时间
    //如果时间大于10分钟，那么丢弃

    //首先对Qlist进行排序 病区 工号 组号
    //然后查某一组的data的长度小于128B
    findItFlag = 0;
    for(it = pFingerBuff->begin(); it != pFingerBuff->end(); it++)
    {
        if(it->data.size() < 128)
        {
            findItFlag = 1;
            //暂存一下
            lastGroup = it->groupId;
            bingquId = it->bingquId;
            yihuId = it->yihuId;
            break;
        }
    }

    if(findItFlag == 1)
    {
        okFlag = 1;

        for(counter = 1; counter <= lastGroup; counter++)
        {
            findItFlag = 0;
            for(it1 = pFingerBuff->begin(); it1 != pFingerBuff->end(); it1++)
            {
                if(it1->bingquId == bingquId &&
                   it1->yihuId == yihuId &&
                   it1->groupId == counter)
                {
                    findItFlag = 1;
                    break;
                }
            }

            //此次组号找到了 1234全部是与的关系
            okFlag &= findItFlag;
        }
    }

    if(okFlag == 1)
    {
        //得到此data的病区和工号和组号
        //然后倒查 相同病区 相同工号 1-n组号的数据是否存在
        //如果存在，那么合并数据，写入数据库
        //如果不存在，不处理，因为可能没有到达，由时间来处理
        for(counter = 1; counter <= lastGroup; counter++)
        {

            for(it1 = pFingerBuff->begin(); it1 != pFingerBuff->end(); it1++)
            {
                if(it1->bingquId == bingquId &&
                   it1->yihuId == yihuId &&
                   it1->groupId == counter)
                {
                    totalData +=  it1->data;
                }
            }

        }
        //合并以上的所有项目 等到的结果在totalData中
        record.setValue("id",totalData);
        fingerModule->model->insertRecord(-1, record);
        fingerModule->submitButtonFunc();//模拟点一下提交修改
    }

}

void SetCallForMir(int data)    //请求标志位0：无 1：有
{
    callForMir = data;
}
int GetCallForMir()    //请求标志位0：无 1：有
{
    return callForMir;
}

QString GetCalledMir() //具体信息 flag0：哪个机器人被叫了 flag1：无意义
{
    return calledMir;
}
void ResetCalledMir()
{
    calledMir.clear();
}

QList <BoxItem> * GetBoxList()
{
    return &boxList;
}
QList <TcpItem> * GetTcpList()
{
    return &socketList;
}
QList <DATA_SYS_DATA> * GetSysStatusTable()
{
    return &sysStateTable;
}

QString GetTaskName()
{
    return taskName;
}
