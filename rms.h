#ifndef RMS_H
#define RMS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QTime>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

#include "tcpsocket.h"
#include "tcpserver.h"

#include "sqlmodule.h"

#include "httplist.h"
#include "displaydebug.h"
#include "table.h"
#include "frame.h"
#include "session.h"
#include "echo/rmswindow.h"

#include "sql/bingqutablecon.h"
#include "sql/boxtablecon.h"
#include "sql/recordtablecon.h"
#include "sql/yihutablecon.h"
#include "boxunit.h"

#include "appconfig/sysconfig.h"

#include "chatserver/chatserver.h"
#include "excel/excelengine.h"

class RMS : public QObject
{
    Q_OBJECT
    
public:
    RMS(QObject *parent = nullptr);
    ~RMS();

    /*单例依赖存放处,所有的单例全部在这里实例化，以便于后期释放*/
    /*配置文件*/
    SysConfig * sysConfig = nullptr;

    /*内存数据表格相关*/
    Table * dataTable = nullptr;

    /*sqlite数据库相关*/
    SqlModule * bingquModule = nullptr;
    SqlModule * yihuModule = nullptr;
    SqlModule * zhiwenModule = nullptr;
    SqlModule * xiangtiModule = nullptr;

    /*数据库单例控制类*/
    BingquTableCON * bingquTableCON = nullptr;
    BoxTableCON * boxTableCON = nullptr;
    RecordTableCON * recordTableCON = nullptr;
    YihuTableCON * yihuTableCON = nullptr;

    /*调度列表，也就是MIR列表*/
    QMap<int,BoxUnit *> boxMap;
    BoxUnit * GetBoxUnit(int boxNum);   //get boxMap item
    QStringList GetAllBoxKeys();    //get all boxMap key(QString)
    bool IsBoxIdExist(int boxid);

    /*socket通信相关*/
    QTimer * demoTimer = nullptr;
    TcpServer * demoTcpServer = nullptr;
    /*socket通信相关:平板*/
    ChatServer * chatServer = nullptr;

    /*串口通信相关*/
    Session * session;
    MSerialPort * mSerialPort = nullptr;

    /*excel相关*/
    ExcelEngine * excelEngine = nullptr;

    /*显示相关*/
    DisplayDebug * demoDisplay= nullptr;
    RmsWindow * demoWindow = nullptr;

public slots:
    void UpdateStockFromConfig();
    void UpdateFromBoxTable();          //DB to boxMap
    void TimerCallBack();
    void SysTimerTask();
};



#endif // RMS
