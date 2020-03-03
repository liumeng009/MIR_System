#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include "controller/boxcon.h"
#include "itemdef.h"
#include "applyform/applyjson.h"

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
    , releaseTimer(new QTimer(this))
{
    //连接socket的
    //1.有数据来
    //2.被动断开连接
    //3.错误信息
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);
    // forward the disconnected and error signals coming from the socket
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendJson(const QJsonObject &json)
{
    // we crate a temporary QJsonDocument forom the object and then convert it
    // to its UTF-8 encoded version. We use QJsonDocument::Compact to save bandwidth
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);

    // we notify the central server we are about to send the message
    emit logMessage("Sending to " + userName() + " - " + QString::fromUtf8(jsonData));

    // we send the message to the socket in the exact same way we did in the client
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}
/*
 * ProcessJson处理每一次有效接收
 * 1.判断请求type
 * 2.实例化BoxCON，对请求提供服务
 * 3.服务返回结果 cmdbox 信号 type result
 * 4.根据box的结果，生成json,回复给chatClient
*/
void ServerWorker::ProcessJson(QJsonObject docObj)
{
    //解析json
    qDebug()<<docObj;
    ApplyJson applyJson;
    const QJsonValue typeVal = docObj.value(QStringLiteral("type_id"));
    userBingqu = docObj.value(QStringLiteral("bingqu")).toString();
    qDebug()<<userBingqu;
    if (typeVal.isNull())
    {
        qDebug()<<"chat json error";
        return; // a message with no type was received so we just ignore it
    }

    //准备子部件 同时指定父对象
    BoxCON * boxCON = BoxCONFactory::instance(this);

    /*获取子部件服务 以下宏定义针对的是JSON通信，来决定调用boxCON的何种服务*/
    /* JSON_TYPE_SUBMIT         1   提交订单命令 平板提交订单
     * JSON_TYPE_CANCEL         2   取消订单命令 平板取消已叫转运箱
     * JSON_TYPE_STATUS_QUERY   3   请求当前状态 平板请求获得转运箱运行状态和请求状态
     * JSON_TYPE_STOCK_QUERY    4   获得库存
     * JSON_TYPE_TIMEOUT        5   超时
    */
    /*可以直接从json中提取”想要的“参数, 后期要判断参数是否存在*/
    switch (typeVal.toInt()) {

    case JSON_TYPE_SUBMIT:
        connect(boxCON,SIGNAL(CmdBoxFinished(int,bool,QVariant,QString)),
                this,SLOT(CmdBoxFinished(int,bool,QVariant,QString)));
        /*判断json字段是否存在。。。todo*/
        applyJson.ApplyJsonMessage = docObj.value(QStringLiteral("applyForm")).toObject();
        qDebug()<<"conetnt:"<<applyJson.JsonToQstring();
        boxCON->RequestInsert(docObj.value(QStringLiteral("request_type")).toInt(0),
                              docObj.value(QStringLiteral("bingqu")).toString(),
                              applyJson.JsonToQstring());
        /***调用服务，等待槽函数被响应***/
        break;

    case JSON_TYPE_CANCEL:
        connect(boxCON,SIGNAL(CmdBoxFinished(int,bool,QVariant,QString)),
                this,SLOT(CmdBoxFinished(int,bool,QVariant,QString)));
        /*判断json字段是否存在。。。todo*/
        boxCON->RequestDelete(docObj.value(QStringLiteral("bingqu")).toString());
        /***调用服务，等待槽函数被响应***/
        break;

    case JSON_TYPE_STATUS_QUERY:
        connect(boxCON,SIGNAL(CmdBoxFinished(int,bool,QVariant,QString)),
                this,SLOT(CmdBoxFinished(int,bool,QVariant,QString)));
        /*判断json字段是否存在。。。todo*/
        boxCON->RequestStatusQuery(docObj.value(QStringLiteral("bingqu")).toString());
        /***调用服务，等待槽函数被响应***/
        break;

    case JSON_TYPE_STOCK_QUERY:
        connect(boxCON,SIGNAL(CmdBoxFinished(int,bool,QVariant,QString)),
                this,SLOT(CmdBoxFinished(int,bool,QVariant,QString)));
        /*判断json字段是否存在。。。todo*/
        boxCON->RequestStockQuery(docObj.value(QStringLiteral("bingqu")).toString());
        /***调用服务，等待槽函数被响应***/
        break;

    default:
        break;
    }
}
void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

/*
 * BOX_REQUEST_INSERT       1
 * BOX_REQUEST_DELETE       2
 * BOX_REQUEST_STATUS_QUERY 3
 * BOX_REQUEST_STOCK_QUERY  4
 * BOX_REQUEST_TIMEOUT      5
*/
/*这是针对boxCON的类型*/
void ServerWorker::CmdBoxFinished(int type, bool result, const QVariant content, const QString detail)
{
    QJsonObject message;
    message["bingqu"] = userBingqu;
    message["detail"] = detail;
    //根据结果构建json
    switch (type) {
    case BOX_REQUEST_INSERT:
        message["type_id"] = JSON_TYPE_SUBMIT;
        if(result)
        {
            message["status_code"] = 200;
        }
        else
        {
            message["status_code"] = 201;
        }
        break;
    case BOX_REQUEST_DELETE:
        message["type_id"] = JSON_TYPE_CANCEL;
        if(result)
        {
            message["status_code"] = 200;
        }
        else
        {
            message["status_code"] = 201;
        }
        break;
    case BOX_REQUEST_STATUS_QUERY:
        message["type_id"] = JSON_TYPE_STATUS_QUERY;
        if(result)
        {
            message["status_code"] = 200;
            message["targetBingqu"] = content.value<StateData>().MIRTargetBingqu;
            message["targetBingquState"] = content.value<StateData>().MIRState;
            message["requestState"] = content.value<StateData>().MIRState;
            message["requestExist"] = content.value<StateData>().requestExist;
        }
        else
        {
            message["status_code"] = 201;
        }
        break;
    case BOX_REQUEST_STOCK_QUERY:
        message["type_id"] = JSON_TYPE_STOCK_QUERY;
        if(result)
        {
            message["status_code"] = 200;
            //content 是 QString
            QString stock = content.toString();
            message["stock"] = QJsonDocument::fromJson(stock.toUtf8()).object();
        }
        else
        {
            message["status_code"] = 201;
            message["stock"] = QJsonObject();   //空
        }
        break;
    case BOX_REQUEST_TIMEOUT:
        message["type_id"] = JSON_TYPE_TIMEOUT;
        if(result)
        {
            message["status_code"] = 202;
        }
        else
        {
            message["status_code"] = 202;
        }
        break;
    default:
        break;
    }

    //发送json
    sendJson(message);
}
QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
}

void ServerWorker::receiveJson()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;

    // create a QDataStream operating on the socket
    QDataStream socketStream(m_serverSocket);

    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_7);

    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();

        // we try to read the JSON data
        socketStream >> jsonData;

        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;

            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {

                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                {
                    emit jsonReceived(jsonDoc.object()); // send the message to the central server

                    //处理
                    ProcessJson(jsonDoc.object());
                }
                else
                    emit logMessage("Invalid message: " + QString::fromUtf8(jsonData)); //notify the server of invalid data

            } else {
                emit logMessage("Invalid message: " + QString::fromUtf8(jsonData)); //notify the server of invalid data
            }
             // loop and try to read more JSONs if they are available

        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}


