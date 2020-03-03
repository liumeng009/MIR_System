#include "tcpserver.h"
#include <QNetworkProxy>

TcpServer::TcpServer(QObject *parent): QTcpServer(parent)
{
    /*禁用操作系统本来的网络代理设置*/
    QNetworkProxyFactory::setUseSystemConfiguration(false);

    demoTimer = new QTimer(this);
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(LinkStateCheck()));
    demoTimer->start(100);  //每100ms执行一次

    debugTimer = new QTimer(this);
    debugTimer->start(10);  //每2000ms执行一次
    connect(debugTimer,SIGNAL(timeout()),this,SLOT(CheckTxBufferValid()));
}
TcpServer::~TcpServer()
{
}

/*一旦有连接来，就在这里处理*/
void TcpServer::incomingConnection(qintptr handle)
{
    //超出最大连接
    if (TcpItemList->size() > maxPendingConnections())
    {
        TcpSocket tcp;
        tcp.setSocketDescriptor(handle); //handle获得socket
        tcp.disconnectFromHost();
        return;
    }

    //实例化后设置handle，统一在QList里管理
    /*实例化一个接受socket，并简单初始化*/
    TcpSocket * client_socket = new TcpSocket(handle);

    //创建一个TcpItem的对象，并初始化，全部值传递
    TcpItem clientItem;             /*tcp转化为QList项，准备插入QList*/
    //item初始化
    clientItem.socketDesciptor = handle;
    clientItem.tcpSocketPtr = client_socket;
    clientItem.utc = QDateTime::currentDateTime().toTime_t();
    clientItem.rawInputData.clear();

    TcpItemList->append(clientItem); //值传递
}
void TcpServer::LinkStateCheck()
{
    TcpSocket * tempSocket;        //临时操作socket指针

    if(TcpItemList->size() == 0)
        return;

    for(int i = 0; i < TcpItemList->size(); ++i)
    {
        //qDebug()<<"list"<<"key:"<<it.key()<<"value:"<<it.value();
        //qDebug()<<TcpItemList.at(i).socketDesciptor;
        //输出看一下
        //tempSocket = TcpItemList->at(i).tcpSocketPtr;
        /*并且判断此链接的任务是存在，如不存在才可以删除*/
        //qDebug()<<tempSocket->handle<<tempSocket->pos();
        //qDebug()<<tempSocket->state();

    }

    for(int i = 0; i < TcpItemList->size(); i++)
    {
        tempSocket = TcpItemList->at(i).tcpSocketPtr;
        /*并且判断此链接的任务是存在，如不存在才可以删除*/
        if(tempSocket->state() != QTcpSocket::ConnectedState)      //连接状态
        //if(tempSocket->state() == QTcpSocket::UnconnectedState)   //未连接状态
        {
            //析构掉socketList中所有申请的内存
            tempSocket->disconnectFromHost();
            qDebug()<<TcpItemList->at(i).socketDesciptor<<"delete";
            TcpItemList->removeAt(i);    //删除记录
            delete tempSocket;          //删除记录后删除对象
            break;  //跳出for 每次处理一个
        }
        else
        {
            if(QDateTime::currentDateTime().toTime_t() - TcpItemList->at(i).utc > 300)
            {
                //析构掉socketList中所有申请的内存
                tempSocket->disconnectFromHost();
                qDebug()<<TcpItemList->at(i).socketDesciptor<<"delete";
                TcpItemList->removeAt(i);    //删除记录
                delete tempSocket;          //删除记录后删除对象
                break;  //跳出for 每次处理一个
            }
        }
    }
}

/*用于debug调试*/
void TcpServer::UpdateText()
{
    QString temp;
    int temp2;
    QByteArray byteTemp;

    if(TcpItemList->size() == 0)
        return;

    for(int i = 0; i < TcpItemList->size(); i++)
    {

        /*数字转化为QString*/
        temp += "socket:"+ QString::number(TcpItemList->at(i).socketDesciptor, 10) + " ";
        temp2 = BoxItemList->at(i).boxID.toInt();
        temp += "boxID:" + QString::number(temp2, 10)+ " ";
        temp2 = BoxItemList->at(i).boxStatus;
        temp += "boxStatus:" + QString::number(temp2, 10) + " ";
        //memcpy(byteTemp.data(),BoxItemList->at(i).MIRIp, 40);
        //temp += "ip:" + byteTemp.toStdString();
        //memcpy(byteTemp.data(),BoxItemList->at(i).MIRPort, 10);
        //temp += "port:" + byteTemp.toStdString().c_str();
        temp2 = BoxItemList->at(i).MIRStatus;
        temp += "MIRStatus:" + QString::number(temp2, 10) + " ";
        //temp += "boxID:" + BoxItemList->at(i).expressList;

        temp += '\n';
    }

    //打印调试
    qDebug()<<temp;
}

/*检查list[0]数据的使用者存在，如果不存在，就会造成严重错误 10ms执行一次*/
void TcpServer::CheckTxBufferValid()
{
    qintptr handleFor0;
    int findItFlag = 0;     //初始化为未找到
    QList <TcpItem>::iterator itTcp;

    if(GetTxBuffer()->size() == 0)
        return;

    //读取0号数据
    handleFor0 = GetTxBuffer()->at(0).socketDesciptor;

    //确定0号数据的socket在tcplist中
    if(TcpItemList->size() == 0)
    {
        findItFlag = 0;
    }
    else
    {
        /*遍历tcp列表，看看有没有针对此handle的通道*/
        for(itTcp = TcpItemList->begin();
            itTcp != TcpItemList->end();
            itTcp ++)
        {
            if(itTcp->socketDesciptor == handleFor0)
                findItFlag = 1;
        }
    }

    //如果不在就删除0号数据
    if(findItFlag == 0)
    {
        //没找到 任务数据无意义 删除此项
        GetTxBuffer()->removeAt(0);
    }
    else
    {
        //找到了由各个任务处理
    }

}
void TcpServer::SetTcpList(QList <TcpItem> * dempPtr)
{
    TcpItemList = dempPtr;
}
void TcpServer::SetBoxList(QList <BoxItem> * dempPtr)
{
    BoxItemList = dempPtr;
}
