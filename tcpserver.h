#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include "QTcpServer"
#include "QTcpSocket"
#include "QMap"
#include "QDebug"
#include "tcpsocket.h"
#include "QTimer"
#include "table.h"
#include <QList>
#include "SocketModel.h"
#include "TableView.h"
#include <QTextEdit>

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    TcpServer(QObject * parent = nullptr);
    ~TcpServer();

    QTimer * demoTimer;
    QTimer * debugTimer;

    //基本数据结构
    QList <TcpItem> * TcpItemList;  //所有连接的表格
    QList <BoxItem> * BoxItemList;  //所有转运箱的表格

    void SetTcpList(QList <TcpItem> * dempPtr);
    void SetBoxList(QList <BoxItem> * dempPtr);
public slots:

    void LinkStateCheck();  /*周期执行检查socket连接状态*/
    void UpdateText();
    void CheckTxBufferValid();
protected:

    virtual void incomingConnection(qintptr handle);

};

#endif // TCPSERVER_H
