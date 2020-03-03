#ifndef FINGERMODULE_H
#define FINGERMODULE_H

#include <QObject>

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

class FingerModule
{
public:
    FingerModule();

    /*请求标志位 当界面触发时，值为1，操作时，值为0*/
    uint16_t UID;
    uint8_t frRequest;
    uint8_t ftRequest;
    uint8_t fvRequest;
    uint8_t fzRequest;
    uint8_t faRequest;
    QString statement;  //指令说明

public:
    void FinReadRunParam(); //运行初始化
    void FinReadIndex();    //读取指纹index
    void FinReadPrint();
    void FinPrintRegister(uint16_t wUID);
    void FinPrintClear();

};

#endif // FINGERMODULE_H
