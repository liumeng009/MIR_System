#include "baseapi.h"
#include <QNetworkInterface>

BaseAPI::BaseAPI(QObject *parent) :
    QObject(parent)
{
    /*
    QString text;
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
            if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
            {
                text =  hostAddress.toString();
            }
    */
//    netProxy.setType(QNetworkProxy::HttpProxy);
//    netProxy.setHostName("127.0.0.1");
//    netProxy.setPort(8888);
//    networkAccessManager.setProxy(netProxy);

    //httpRequest.setRawHeader("Accept", API_ACCEPT);
    //httpRequest.setRawHeader("User-Agent",API_USER_AGENT);
    //httpRequest.setRawHeader("X-XXX-API-Key",API_KEY);
    //httpRequest.setRawHeader("X-XXX-API-Secret",API_SECRET);
    //httpRequest.setRawHeader("Accept-Encoding","gzip,deflate");

    httpRequest.setRawHeader("Accept", "text/plain, text/html");
    httpRequest.setRawHeader("Accept-Language", "en,zh");
    httpRequest.setRawHeader("Content-Type", "application/json");
    httpRequest.setRawHeader("Authorization", "Basic ZGlzdHJpYnV0b3I6NjJmMmYwZjFlZmYxMGQzMTUyYzk1ZjZmMDU5NjU3NmU0ODJiYjhlNDQ4MDY0MzNmNGNmOTI5NzkyODM0YjAxNA==");

    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(serviceRequestFinished(QNetworkReply*)));

//    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),
//            this,SIGNAL(UploadReply(QNetworkReply*)));
}

void BaseAPI::get(const QString url)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.get(httpRequest);
}

void BaseAPI::post(const QString url, const QByteArray &data)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.post(httpRequest, data);
}

void BaseAPI::put(const QString url, const QByteArray &data)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.put(httpRequest, data);
}

void BaseAPI::deleteIt(const QString url)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.deleteResource(httpRequest);
}
//接收中断
void BaseAPI::serviceRequestFinished(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(statusCode != 200)
    {
        qDebug()<<"通信异常...serviceRequestFinished..statusCode"<<reply->errorString()<<statusCode;
        qDebug()<<reply->header(QNetworkRequest::ContentTypeHeader).toString();
    }

    emit UploadReply(reply);

    if(reply->error() == QNetworkReply::NoError)
    {
        //requestFinished(reply, reply->readAll(), statusCode);
        //emit UploadReply(reply);
    }
    else
    {
        //requestFinished(reply,"",statusCode);
        //reply->deleteLater();
    }

    /*无论如何都先发送出去,由httplist来delete*/

    //reply->deleteLater();
}
