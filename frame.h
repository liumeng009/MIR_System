#ifndef FRAME_H
#define FRAME_H

#include <QList>
#include <QByteArray>
#include <QtEndian>
#pragma pack(push,1)
typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint8_t group;
    uint8_t groupNum;
    uint16_t crc16;
}DATA_REQUEST;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;

    /*+变长处理*/
}DATA_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;    /*固定长度*/
    uint16_t boxId;
    uint16_t crc16;
}DATA_EXPRESS_REPORT;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint16_t boxId;
    uint16_t MIRStatus;
    uint16_t bingqu;
    uint16_t crc16;
}DATA_MIR_POSITION_REQUEST_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint16_t boxId;
    uint32_t UTC;
    uint16_t version;
    uint16_t crc16;
}DATA_HEARTBEAT_REQUEST_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint16_t boxId;
    uint8_t managePassword[10];
    uint8_t operatePassword[10];
    uint8_t openBoxPassword[10];
    uint16_t crc16;
}DATA_PASSWORD_REQUEST_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint16_t bingquId;
    uint16_t yihuId;
    uint8_t groupNum;
    uint16_t crc16;
}DATA_FINGER_REQUEST;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    /*+变长处理*/
}DATA_FINGER_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint8_t bingqu;
    uint16_t gonghao;
    uint8_t group;
    uint8_t data[128];
    uint16_t crc16;
}DATA_FINGER_REPORT;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint8_t group;
    uint8_t recvLength;
    uint16_t crc16;
}DATA_FINGER_REPORT_ACK;

typedef struct
{
    uint16_t header;
    uint16_t cmd;
    uint16_t length;
    uint16_t boxId;
    uint16_t crc16;
}DATA_START_CMD;

/*收到readall()数据后，就放入buff中自动解析
根据cmd的值，判断是哪种请求，随后进行定长或变长处理*/
typedef union
{
    DATA_REQUEST generalData;
    DATA_EXPRESS_REPORT expressData;
    DATA_MIR_POSITION_REQUEST_ACK MIRData;
    DATA_HEARTBEAT_REQUEST_ACK heartbeatData;
    DATA_PASSWORD_REQUEST_ACK passwordData;
    DATA_FINGER_REQUEST fingerRequestData;
    DATA_FINGER_REPORT fingerReportData;
    DATA_START_CMD     startCmdData;
    uint8_t buff[200];  //由最长的决定
}REQUEST;


/*定义接收数据的基本项*/
typedef struct
{
    int socketDesciptor;
    QByteArray rxData;
}RX_DATA_ITEM;

typedef struct
{
    int socketDesciptor;
    QByteArray txData;
}TX_DATA_ITEM;

QList <RX_DATA_ITEM> * GetRxBuffer();
QList <TX_DATA_ITEM> * GetTxBuffer();

uint16_t Get_Crc16(uint8_t *puchMsg,uint16_t usDataLen);

typedef struct
{
    uint8_t bingquId;   //病区id
    uint16_t yihuId;    //医护id
    uint8_t groupId;    //组号
    QByteArray data;    //指纹部分数据
    uint32_t timeStamp; //时间戳
}FINGER_DATA_ITEM;

QList <FINGER_DATA_ITEM> * GetFingerBuffer();

#pragma pack(pop)
#endif // FRAME_H
