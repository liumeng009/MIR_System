#ifndef TABLE_H
#define TABLE_H

#include <QString>
#include <QList>
#include <QSqlRecord>
#include <QTimer>
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>
#include <QByteArray>
#include <QSerialPort>
#include "tcpsocket.h"
#include "sqlmodule.h"
#include "sql/bingqutablecon.h"
#include "sql/yihutablecon.h"

/*单个http GET POST，也就是从单个MIR机器人获取的表格*/
/*mcu上传的任务列表*/
typedef struct
{
    uint16_t taskBingquId;
    QString name;       //以下查表病区表和自己的positonGuidList获得
    QString guid;
    QString map;
    double X;           //固定
    double Y;           //固定
    double distance;    //与当前位置的距离
    uint8_t range;      //在里面 不在里面 实时更新
    uint8_t alreadyReached;//到过了没有
    QString url;
    int timeout;        //单个节点计时 单位s
}ExpressItem;
/*MIR机器人上传的可选任务号*/
typedef struct
{
    QString guid;
    QString name;
    QString url;
}MISSION_INFO;
/*MIR机器人上传的全部地址信息*/
typedef struct
{
    QString name;
    QString guid;
    QString map;
    double X;      //二次请求得到每个机器人的位置的坐标
    double Y;
    QString url;
}POSITION_INFO;

typedef struct
{
    QString MIRTargetBingqu;    //当前目标bingqu
    int MIRState;               //针对目标bignqu的状态 1:正执行 2：已到达
    QString stateId;            //http刷新 当前状态编号
    QString stateText;          //http刷新 当前状态文本
    QString mapId;              //http刷新 当前机器人所在的mapId
    QString missionText;        //http刷新 当前任务文本状态*/
    QString missionState;
    double battery_percentage;
    int battery_time_remaining;
    double pos_x;          //http刷新 当前x坐标
    double pos_y;          //http刷新 当前y坐标
}STATE_INFO;

/*单个http GET POST，也就是从单个MIR机器人获取的表格*/

/***单纯处理socket连接***/
typedef struct
{
    int socketDesciptor;        //主键 tcpServer产生
    TcpSocket * tcpSocketPtr;   //指针不用关心 tcpServer产生
    QByteArray rawInputData;    //原始数据接收缓冲区
    uint utc;                   //判断连接有效性
}TcpItem;
/***这是由箱体配置表生成的内存表格，主要目的是动态存储机器人参数信息***/
typedef struct
{
    QString boxID;              //箱体编号(主键) 0代表无效 socket通信刷新

    uint8_t boxStatus;          //箱体心跳状态    socket通信刷新
    uint8_t MIRIp[40];          //绑定的ip       数据库刷新
    uint8_t MIRPort[10];        //绑定的port     数据库刷新
    int MIRStatus;              //机器人状态 哪个病区 http刷新
    QList <ExpressItem> * expressList; //任务单 列表指针  socket通信刷新
                 /*这样结构体可以值复制，指针只保存一份实例*/

    //机器人部分
    /*getMission*/
    QString url;     //数据库刷新，http使用
    QString port;    //数据库刷新，http使用
    QString guid;    //由http通信+json产生，lineedit使用
    QList <MISSION_INFO> * missionGuidList;    //请求后，由http+json解析出来
    QString missionIdSelected;  //目前任意指定

    /*putMissionQueue*/
    uint8_t httpRequest;    /*请求标志位，置1执行请求，置0不执行请求*/
    QString message;    //无用
    QString missionid;  //由mission表产生
    QString para1;      //程序产生 起始地 一般不用
    QString para2;      //程序产生 结束地
    float   priority;   //0任务最高 充电1

    /*getStatus*/
    QString position1;
    QString stateId1;       //http刷新 当前状态编号
    QString stateText1;     //http刷新 当前状态文本
    QString mapId;          //http刷新 当前机器人所在的mapId
    QString missionText;    //http刷新 当前任务文本状态*/
    double pos_x;          //http刷新 当前x坐标
    double pos_y;          //http刷新 当前y坐标

    QString currentBingqu;  /*要和MCU通信的，有missionstate和target决定*/

    uint16_t targetBingquId;/*和林总通信 当前位置相关*/
    QString targetBingqu;
    uint8_t targetBingquRange;

    int execCounter;        /*发车动作执行序列 1；删除任务队列 2 status ready 3 添加任务队列*/

    QString previousMissionState;   /*由missionState更新，便于判断机器启动和停止的瞬间*/
    uint8_t missionStateChange;     /*任务状态改变标志位 0无 1wait到work 2work到wait*/
    QString missionState;   /*由missionText产生 httplist接收的时候就处理了 waiting working*/
    uint8_t missionNum;     /*根据socketlist判断*/

    /*postStatus*/
    QString setStateId;     /*要设置的状态*/
    QString position2;      /*返回的状态*/
    QString stateId2;
    QString stateText2;

    /*GetPosition*/
    int positionStatus; //地址列表完成标志 1；完成 2；未完成
    QList <POSITION_INFO> * casheGuidList;    //周期位置地图列表，查找地名，从中取出guid
    QList <POSITION_INFO> * positonGuidList;    //位置地图列表，查找地名，从中取出guid
    QString missionPositionSelected; //positonGuidList产生，根据目标病区产生
}BoxItem;

/********数据库更新的数据格式*********/
typedef struct  /*全部传送*/
{
    uint16_t id;
    uint8_t name[16];
    QString strName;
}DATA_BINGQU;
typedef struct  /*全部传送*/
{
    uint16_t id;
    uint8_t name[8];
    uint16_t bingquId;
    QString strName;
    QString strFinger;
}DATA_YIHU;
typedef struct /*全部传送*/
{
    uint16_t id;
    uint8_t name[500];
}DATA_FINGER;
typedef struct
{
    QString id;
    QString ip;
    QString port;
}DATA_XIANGTI;
/********数据库更新的数据格式*********/


/*病区ID 病区名称 状态 可见*/
typedef struct
{
    QString bingquId;
    QString bingquName;
    uint8_t state;
    //uint8_t visible;
}DATA_SYS_ITEM;

typedef struct
{
    QString boxId;                  //箱体号
    QString taskText;               //任务列表
}DATA_SYS_DATA;

class Table : public QObject
{
    Q_OBJECT

public:
    Table(QObject * parent = nullptr);
    ~Table();

    //配置
    QSettings * configIni;


    /*定义各种表格,正式这些表格作为软件的基础运行*/
    //...全部对象可见,一定要初始化！！！否则就无法实例化，就不是static
    QDateTime * dateTime;
    QTimer * demoTimer;
    QTimer * dataTimer;

    /*数据库相关*/
    QList<QSqlRecord> * bingquRecordList;
    QList<QSqlRecord> * yihuRecordList;
    QList<QSqlRecord> * fingerRecordList;
    QList<QSqlRecord> * xiangtiRecordList;

    void SetBingquTable(QList<QSqlRecord> * demoPtr);
    void SetYihuTable(QList<QSqlRecord> * demoPtr);
    void SetFingerTable(QList<QSqlRecord> * demoPtr);
    void SetXiangtiTable(QList<QSqlRecord> * demoPtr);


    //struct 数组定义所需要的表格参数 （值传递 读取出来的）
    QList <DATA_BINGQU>  bingquTable;
    QList <DATA_YIHU>  yihuTable;
    QList <DATA_FINGER>  fingerTable;
    QList <DATA_XIANGTI>  xiangtiTable;

    SqlModule * fingerModule;
    void SetFingerModule(SqlModule * demoPtr);

    /*数据库控制类*/
    BingquTableCON * bingquTableCON = nullptr;
    YihuTableCON * yihuTableCON = nullptr;

public slots:
    void BingquTableUpdate();
    void YihuTableUpdate();
    void FingerTableUpdate();
    void XiangtiTableUpdate();

    void SysStatusTableUpdate();
    void CallMirStatusUpdate();

    /*表格到数据帧的处理函数*/
    void RxBuff2TcpBuff();
    void TcpBuffVertify();
    void TcpFrameProcess(int socketDesciptor, QByteArray dataByteArray);

    void SocketTaskProcess();

    /*XiangtiTable到boxlist的更新*/
    void BoxListUpdate();

    /*指纹数据输入到指纹表的model中*/
    void FingerDataCheck();
    
    /*配置update*/
    void ConfigUpdate();

};

/*全局变量区*/
extern int loginValid;

/*供外部调用API*/
void SetCallForMir(int data);
int GetCallForMir();
QString GetCalledMir();
void ResetCalledMir();
QList <BoxItem> * GetBoxList();
QList <TcpItem> * GetTcpList();
QList <DATA_SYS_DATA> * GetSysStatusTable();
QString GetTaskName();



#endif // TABLE_H
