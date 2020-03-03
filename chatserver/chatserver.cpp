#include "chatserver.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

ChatServer * ChatServerFactory::m_pInstance = nullptr;

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent)
{

}
void ChatServer::toggleStartServer()
{
    if (isListening()) {
        stopServer();
        logMessage(QStringLiteral("Server Stopped"));
        qDebug()<<"chatserver stop!";
    } else {
        if (!listen(QHostAddress::Any, 7000)) {
            //QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
            return;
        }
        logMessage(QStringLiteral("Server Started"));
        qDebug()<<"chatserver start...";
    }
}
void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    /*实例化一个worker，这个worker注册一下保存，
    *这个worker负责以后boxcon的实例化与操作，因为他还要通信返回
    *他的释放主要是由客户端负责，
    * worker本身也要计时释放*/
    ServerWorker *worker = new ServerWorker(this);
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&ChatServer::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&ChatServer::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);
    m_clients.append(worker);
    emit logMessage(QStringLiteral("New client Connected"));
}
//通过服务器来发送socket自己的信息
void ChatServer::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination);
    destination->sendJson(message);
}
//所有socket的广播
void ChatServer::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    for (ServerWorker *worker : m_clients) {
        Q_ASSERT(worker);
        if (worker == exclude)
            continue;
        sendJson(worker, message);
    }
}
//服务器接收到socket自己的信息
void ChatServer::jsonReceived(ServerWorker *sender, const QJsonObject &doc)
{
//    Q_ASSERT(sender);
//    emit logMessage("JSON received " + QString::fromUtf8(QJsonDocument(doc).toJson()));
//    if (sender->userName().isEmpty())
//        return jsonFromLoggedOut(sender, doc);
//    jsonFromLoggedIn(sender, doc);
}
//服务器处理socket的断开连接操作
void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if (userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = QStringLiteral("userdisconnected");
        disconnectedMessage["username"] = userName;
        //broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + " disconnected" + QString::number(m_clients.size()));
    }
    sender->deleteLater();
}
//服务器打印socket的错误
void ChatServer::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage("Error from " + sender->userName() + sender->m_serverSocket->errorString());
}
//服务器停止服务
void ChatServer::stopServer()
{
    for (ServerWorker *worker : m_clients) {
        worker->disconnectFromClient();
    }
    close();
}

void ChatServer::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
//    Q_ASSERT(sender);
//    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
//    if (typeVal.isNull() || !typeVal.isString())
//        return;
//    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) != 0)
//        return;
//    const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
//    if (usernameVal.isNull() || !usernameVal.isString())
//        return;
//    const QString newUserName = usernameVal.toString().simplified();
//    if (newUserName.isEmpty())
//        return;
//    for (ServerWorker *worker : qAsConst(m_clients)) {
//        if (worker == sender)
//            continue;
//        if (worker->userName().compare(newUserName, Qt::CaseInsensitive) == 0) {
//            QJsonObject message;
//            message["type"] = QStringLiteral("login");
//            message["success"] = false;
//            message["reason"] = QStringLiteral("duplicate username");
//            sendJson(sender, message);
//            return;
//        }
//    }
    //sender->setUserName(newUserName);
//    QJsonObject successMessage;
//    successMessage["type"] = QStringLiteral("login");
//    successMessage["success"] = true;
    //sendJson(sender, successMessage);
//    QJsonObject connectedMessage;
//    connectedMessage["type"] = QStringLiteral("newuser");
//    connectedMessage["username"] = newUserName;
//    broadcast(connectedMessage, sender);
}

void ChatServer::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
{
//    Q_ASSERT(sender);
//    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
//    if (typeVal.isNull() || !typeVal.isString())
//        return;
//    if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) != 0)
//        return;
//    const QJsonValue textVal = docObj.value(QLatin1String("text"));
//    if (textVal.isNull() || !textVal.isString())
//        return;
//    const QString text = textVal.toString().trimmed();
//    if (text.isEmpty())
//        return;
//    QJsonObject message;
//    message["type"] = QStringLiteral("message");
//    message["text"] = text;
//    message["sender"] = sender->userName();
    //broadcast(message, sender);
}


