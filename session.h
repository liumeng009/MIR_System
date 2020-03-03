#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include "mserialport.h"

#pragma pack(push,1)

#define CMD_MR 1
#define CMD_MW 2
#define CMD_FT 3
#define CMD_FR 4
#define CMD_FV 5
#define CMD_FZ 6
#define CMD_FD 7
#define CMD_FA 8

#define MAX_FINGER_NUM 100

typedef struct _FINGER_INDEX					// 指纹特征索引
{												// ( sizeof(FINGER_INDEX) % 6 == 0 ) && ( MAX_FINGER_NUM * sizeof(FINGER_INDEX) <= 768u )
    uint16_t	wUID;							// 医护工号
    uint32_t	dwOffset;						// EEPROM 访问偏移量
    uint16_t	wBytesLen;						// 指纹特征数据长度
} FINGER_INDEX;

typedef struct _RUN_PARAM						// 运行参数
{
    uint16_t	wTotalFingerChar;			    // 已存储的指纹特征总数	2
    uint32_t	dwNextValidAddr;				// 指纹特征可用空间起址	4
    FINGER_INDEX fingerIndex[MAX_FINGER_NUM];	// 指纹特征索引			800
} RUN_PARAM;									//	Total			=	806 Bytes

typedef struct _FINGER_ITEM					// 指纹特征索引
{												// ( sizeof(FINGER_INDEX) % 6 == 0 ) && ( MAX_FINGER_NUM * sizeof(FINGER_INDEX) <= 768u )
    uint16_t	wUID;							// 医护工号
    QByteArray  data;
    QString     strData;                        //指纹原始数据
} FINGER_ITEM;
#pragma pack(pop)

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(QObject *parent = nullptr);
    ~Session();

    QString strCmd;     //当前命令是什么
    MSerialPort * serialPort = nullptr;
    QTimer * demoTimer = nullptr;
    QTimer * sessionTimer = nullptr;

    /*界面参数组（就是界面给了多少参数）：请求标志位 当界面触发时 请求1 执行2 完成0*/
    uint16_t UID;

    /*模块内存模型*/
    RUN_PARAM runParam;
    QList <FINGER_ITEM> * fingerList;

    /*session底层命令的参数组*/

    uint8_t funcNum = 0;     //功能代号 1运行参数 2连续指纹读取 3注册指纹
                             //4验证指纹 5配置指纹 6删除 7清空 9停止验证
    uint8_t funcState = 0;  //功能执行状态 0未执行 1开始执行
    uint16_t	fvUID;   //FV
    //QString statement;  //指令说明
    QByteArray statement;
    //上层操作封装：

    //读取运行参数
    bool runParamStart = false;
    //连续读取指纹数据
    uint8_t transactionState = 0;
    bool fingerReadStart = false;
    uint16_t fingerCounter = 0;

    /*session底层命令*/
    void Start(uint8_t cmd, uint16_t addr, uint32_t byteslen);
    void Start(uint8_t cmd, uint16_t UID);

signals:

public slots:
    void RecvData(QByteArray data);

    void FuncExecute();
    void TransactionExecute();

    void SessionTimeout();
    void TestWrite();

    /*功能接收槽*/
    void MrRecvParse(QString data);
    void MrRecvFinParse(QString data);

    /*封装好的指纹命令,执行界面下达的参数+命令*/
    void FinAllDataRead();

    void FinReadRunParam(); //运行初始化 包括指纹索引
    void FinDataRead();
    bool FinReadPrint(uint16_t addr, uint32_t byteslen); /*所有指纹*/
    void FinPrintRegister(uint16_t wUID);   /*注册指纹*/
    void FinPrintVertify();                 /*验证指纹*/
    void FinConfig();
    void FinPrintVertifyStop();             /*停止验证指纹*/
    void FinPrintDelete();                  /*删除指纹*/
    void FinPrintClear();
};

#endif // SESSION_H
