#include "session.h"
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QRegExp>
#include <QTimer>

Session::Session(QObject *parent) :
    QObject(parent),
    serialPort(MSerialPortFactory::instance(this)),
    fingerList(new QList<FINGER_ITEM>())
{
    funcNum = 0;
    funcState = 0;
    //memset(&runParam,0,sizeof(runParam));
    //serialport上传的数据，传到此类中
    connect(serialPort,SIGNAL(Upload(QByteArray)),this,SLOT(RecvData(QByteArray)));

    demoTimer = new QTimer;
    //connect(demoTimer,SIGNAL(timeout()),this,SLOT(TestWrite()));
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(FuncExecute()));
    demoTimer->start(1000);

    sessionTimer = new QTimer;
    connect(sessionTimer,SIGNAL(timeout()),this,SLOT(SessionTimeout()));
}
Session::~Session()
{
    qDebug()<<"serialprot delete later";
    //在这里没有delete serialport ，因为系统会在调用析构函数之前，自动删除此成员所指对象
    demoTimer->deleteLater();
}
/*发送命令一次，代表Session的开始，strCmd写入：1有命令，2是什么命令*/
void Session::Start(uint8_t cmd, uint16_t addr, uint32_t byteslen)
{
    QString strAddr = QString::number(addr,10);
    QString strByteslen = QString::number(byteslen,10);
    QString strCmdExec;

    switch (cmd)
    {
    case CMD_MR:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "mr/" + strAddr + "," + strByteslen;
        strCmdExec = strCmd + "\r\n";
        serialPort->SerialWrite(strCmdExec);
        //开始计时，超时认为异常
        sessionTimer->start(3000);
        break;
    case CMD_MW:
        strCmd = "mw/" + strAddr + "," + strByteslen;
        strCmdExec = strCmd + "\r\n";
        serialPort->SerialWrite(strCmdExec);
        //开始计时，超时认为异常
        sessionTimer->start(1000);
        break;
    default:
        break;
    }
}
void Session::Start(uint8_t cmd, uint16_t UID)
{
    QString strUID = QString::number(UID,10);
    QString strCmdExec;

    switch (cmd)
    {
    case CMD_FT:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "ft";
        break;
    case CMD_FR:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "fr/" + strUID;
        break;
    case CMD_FV:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "fv";
        break;
    case CMD_FZ:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "fz";
        break;
    case CMD_FD:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "fd/" + strUID;
        break;
    case CMD_FA:
        /*记录此刻的命令开始，命令名称*/
        strCmd = "fa";
        break;
    default:
        break;
    }
    /*执行*/
    strCmdExec = strCmd + "\r\n";
    serialPort->SerialWrite(strCmdExec);
    //开始计时，超时认为异常
    sessionTimer->start(1000);
}

/*不管什么类型的命令，返回都是字符串，用一个函数统一处理*/
void Session::RecvData(QByteArray data)
{
    //转化为字符串(字面转化:数字对应unicode，因此)
    QString strRecvData(data);
    QStringList result;
    QString cmdType;

    /*根据ASCII规则转换为字符串*/
    qDebug()<<"原始接收"<<strRecvData;

    //字符串->利用正则分析
    result = strRecvData.split(QRegExp("\\r\\r\\n"));
    //qDebug()<<result[0]<<result[1]<<result[2]<<result[3]<<result[4];
    //qDebug()<<result.length();

    if(result.isEmpty())
        return;

    //首先对“回显命令”和《说明》和“空”进行过滤
    foreach(QString item, result)
    {
        if(item.isEmpty())
        {
            qDebug()<<"删除的空白"<<item;
            result.removeOne(item);
        }
        else if(item.at(0) == '-')
        {
            qDebug()<<"删除的说明"<<item;
            //statement += item + "\n";
            statement = data;
            result.removeOne(item);
        }
        else if(item == strCmd)
        {
            qDebug()<<"删除的命令回显"<<item;
            result.removeOne(item);
        }
    }

    if(result.length() == 0)    //nothing
        return;

    //根据命令的不同处理 根据strCmd提取命令类型
    QRegExp cmdTypeRx("^\\w+");
    cmdTypeRx.indexIn(strCmd);
    cmdType = cmdTypeRx.cap(0);

    /*OK负责将有用的信息返回,但是大部分有用信息直接自己读取*/
    if(result[0] == "+OK")
    {

        //根据命令的不同,不同的命令会有不同的返回
        if(cmdType == "mr")
        {
            /*OK后接一串数据，发送信号给处理*/

            if(funcNum == 1)
            {
                if(result.length() >= 4)
                    MrRecvParse(result[3]); //run参数单独处理函数

                funcNum = 0;
                funcState = 0;
                strCmd.clear(); //清除记忆命令,接下来就处理
            }
            if(funcNum == 2)
            {
                if(result.length() >= 4)
                    MrRecvFinParse(result[3]); //指纹单独处理函数

                funcNum = 0;
                funcState = 0;
                strCmd.clear(); //清除记忆命令,接下来就处理
            }

        }
        if(cmdType == "mw")
        {
            //...
        }

        /*以下命令的返回，不太需要处理*/
        if(cmdType == "ft")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
        if(cmdType == "fr")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
        if(cmdType == "fv")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
        if(cmdType == "fz")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
        if(cmdType == "fd")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
        if(cmdType == "fa")
        {
            /*读取指纹编号*/
            /*读取数据长度*/
            funcNum = 0;
            funcState = 0;
            strCmd.clear(); //清除记忆命令,接下来就处理
        }
    }
    else if(result[0] == "+ERR")
    {
        funcNum = 0;
        funcState = 0;
        strCmd.clear(); //清除记忆命令
    }
    else if(result[0] == "+INVALID")
    {
        funcNum = 0;
        funcState = 0;
        strCmd.clear(); //清除记忆命令
    }
    else
    {
        funcNum = 0;
        funcState = 0;
        //什么都不是 异常
    }

}

/*给外部提供的调用接口，还要判断session是否结束，串口是否可用*/
void Session::FinAllDataRead()
{
    if(serialPort->IsOpen())
    {
        if(fingerReadStart == false)
        {
            fingerCounter = 0;  //参数初始化
            transactionState = 0; //程序状态初始化
            fingerReadStart =  true;
            fingerList->clear();
        }
    }
}
void Session::FinReadRunParam()
{
    //mr/0,806 读取全部头数据
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum =  1;   //读取run参数
            funcState = 0;  //未执行
        }
    }
}
void Session::FinDataRead()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum =  2;   //读取run参数
            funcState = 0;  //未执行
        }
    }
}
void Session::FinPrintRegister(uint16_t wUID)
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            fvUID = wUID;
            funcNum = 3;    //注册指纹
            funcState = 0;
        }
    }
}
void Session::FinPrintVertify()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum = 4;    //注册指纹
            funcState = 0;
        }
    }
}
void Session::FinConfig()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum = 5;    //配置指纹模块
            funcState = 0;
        }
    }
}
void Session::FinPrintDelete()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum = 6;
            funcState = 0;
        }
    }
}
void Session::FinPrintClear()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum = 7;
            funcState = 0;
        }
    }
}
void Session::FinPrintVertifyStop()
{
    if(serialPort->IsOpen())
    {
        if(funcNum == 0)
        {
            funcNum = 8;
            funcState = 0;
        }
    }
}
bool Session::FinReadPrint(uint16_t addr, uint32_t byteslen)
{
    if(serialPort->IsOpen())
    {
        if(strCmd == "")
        {
            Start(CMD_MR, addr , byteslen);
            return true;
        }
    }
    return false;
}

/*专门的接收*/
void Session::MrRecvParse(QString data)
{
    QStringList result;
    QByteArray ba;
    bool ret;

    if(strCmd == "mr/0,806")
    {
        //解析data
        //转换为QByteArray
        //更新结构体
        result = data.split(QRegExp("[ ]"));
        result.removeLast();

        foreach (QString item, result)
        {
            ba.append(item.toInt(&ret,16));
        }
        qDebug()<<"接受的长度"<<ba.length();
        //更新到结构体中
        if(ba.length() == 806)
            memcpy((void *)&runParam,ba.data(),ba.length());
        qDebug()<<"offset"<<runParam.fingerIndex[0].dwOffset;
    }

}
void Session::MrRecvFinParse(QString data)
{
    QStringList result;
    QByteArray ba;
    bool ret;

    //接收到了一个指纹索引提供的指纹信息

    result = data.split(QRegExp("[ ]"));
    result.removeLast();
    foreach (QString item, result)
    {
        ba.append(item.toInt(&ret,16));
    }

    //更新到QList中,此时的counter指向的是当前的索引
    FINGER_ITEM fingerItem;
    fingerItem.wUID = runParam.fingerIndex[fingerCounter].wUID;
    fingerItem.data = ba;
    fingerItem.strData = data.remove(data.length() - 1 ,1);

    fingerList->append(fingerItem);
    qDebug()<<"编号"<<fingerItem.wUID;
    qDebug()<<"指纹"<<fingerItem.data<<"QLIST 长度"<<fingerList->length();
}

void Session::FuncExecute()
{
    /*此时没有功能要执行*/
    //qDebug()<<"funcNum"<<funcNum;
    //qDebug()<<"funcState"<<funcState;

    TransactionExecute();

    if(funcNum == 0)
    {
        funcState = 0;      //确定初始状态
        return;
    }

    if(funcNum == 1 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_MR, 0 , 806); //固定命令
    }

    if(funcNum == 2 && funcState == 0)
    {
        funcState = 1;  //开始执行

        qDebug()<<"______offset"<<runParam.fingerIndex[fingerCounter].dwOffset;
        qDebug()<<"______length"<<runParam.fingerIndex[fingerCounter].wBytesLen;
        /*首先读param，然后进行各个指纹的读取*/
        Start(CMD_MR, runParam.fingerIndex[fingerCounter].dwOffset + 806 ,
              runParam.fingerIndex[fingerCounter].wBytesLen);
    }

    if(funcNum == 3 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FR, fvUID);
    }

    if(funcNum == 4 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FV,0);
    }

    if(funcNum == 5 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FT,0);
    }

    if(funcNum == 6 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FD,fvUID);
    }

    if(funcNum == 7 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FA,0);
    }

    if(funcNum == 8 && funcState == 0)
    {
        funcState = 1;  //开始执行
        Start(CMD_FZ,0);
    }

}
void Session::TransactionExecute()
{
    //qDebug()<<"start"<<fingerReadStart<<"state"<<transactionState;
    //qDebug()<<"num of fin"<<runParam.wTotalFingerChar<<fingerCounter;
    if(fingerReadStart == true)
    {
        switch (transactionState) {
        case 0:
            if(funcNum == 0)
            {
                funcNum = 1;
                transactionState = 1;
            }
            break;
        case 1:
            if(funcNum == 0)    //参数读取执行完毕，开始读取指纹
            {
                if(runParam.wTotalFingerChar == 0)
                {
                    fingerReadStart = false;
                    fingerCounter = 0;
                }
                else
                {
                    funcNum = 2;
                    transactionState = 2;
                }
            }
            break;
        case 2:
            if(funcNum == 0)    //读取执行完毕
            {
                if(fingerCounter < runParam.wTotalFingerChar -1)
                {
                    //指纹没有读取完毕
                    funcNum = 2;
                    fingerCounter++;    //读取下一个
                }
                else
                {
                    //指纹读取完毕
                    fingerReadStart = false;
                    fingerCounter = 0;
                }
            }
            else
            {
                //继续等待
            }
            break;
        default:
            break;
        }
    }
}
void Session::SessionTimeout()
{
    //sessionTimer->stop();

    //清空所有session状态，保持在没有命令执行的状态
    //strCmd.clear();
    //funcNum = 0;
    //funcState = 0;
}


void Session::TestWrite()
{
    qDebug()<<"test";
    if(serialPort->IsOpen())
    {
        //qDebug()<<"open"+strCmd;
        if(strCmd == "")
        {
            qDebug()<<"exec";
            Start(1,0,10);
            //Start(CMD_FR, 1000);
        }

    }
}


