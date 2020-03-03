#include "tcpsocket.h"


TcpSocket::TcpSocket(qintptr handle)
{
    this->setSocketDescriptor(handle);
    this->handle = handle;

    demoTimer = new QTimer(this);
    QObject::connect(demoTimer,SIGNAL(timeout()),this,SLOT(WriteMessage()));

    demoTimer->start(10);
    //有可读的信息，触发读函数槽
    connect(this,SIGNAL(readyRead()),this,SLOT(ReadMessage()));
    connect(this, &QAbstractSocket::disconnected, this,&DisconnectProcess);
}
TcpSocket::TcpSocket()
{
    demoTimer = new QTimer(this);
    QObject::connect(demoTimer,SIGNAL(timeout()),this,SLOT(WriteMessage()));

    demoTimer->start(10);
    //有可读的信息，触发读函数槽
    connect(this,SIGNAL(readyRead()),this,SLOT(ReadMessage()));
}

void TcpSocket::DisconnectSocket(qintptr handle_come)
{
    if(this->handle == handle_come)
        disconnectFromHost();
}
void TcpSocket::DisconnectProcess()
{
    qDebug()<<"warning disconnnect";
}

void TcpSocket::ReadMessage()
{
    RX_DATA_ITEM buffData;

    /*实际接收数据*/
    QByteArray buffer = this->readAll();

    /*做成handle + QbyteArray的形式（插入缓冲需要的格式）*/
    buffData.socketDesciptor = handle;
    buffData.rxData = buffer;           /*填入到结构体*/

    GetRxBuffer()->append(buffData);    /*结构体插入总缓冲*/
}

/*WriteMessage需要周期执行10ms*/
void TcpSocket::WriteMessage()
{
    TX_DATA_ITEM buffData;

    /***************一定要先判断缓冲区是不是0，在进行操作********/
    /*否则at(0)这种操作会引起段错误！！*/
    if(GetTxBuffer()->size() == 0)
        return;

    /***************总缓冲中，是否是自己的数据handle***********/
    if(GetTxBuffer()->at(0).socketDesciptor == handle)
    {
        buffData = GetTxBuffer()->at(0);
        GetTxBuffer()->removeAt(0);
        this->write(buffData.txData.data(),buffData.txData.size());
    }
}
