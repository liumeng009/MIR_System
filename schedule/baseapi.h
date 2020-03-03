#ifndef BASEAPI_H
#define BASEAPI_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkProxy>


class BaseAPI : public QObject
{
    Q_OBJECT
public:
    QNetworkRequest httpRequest;
    QNetworkAccessManager networkAccessManager;
    QNetworkProxy netProxy;

    explicit BaseAPI(QObject *parent = 0);

    void get(const QString url);
    void post(const QString url, const QByteArray &data);
    void put(const QString url, const QByteArray &data);
    void deleteIt(const QString url);

//protected:
    //virtual void requestFinished(QNetworkReply * reply, const QByteArray data, const int statusCode);
signals:
    void UploadReply(QNetworkReply * reply);
public slots:
    void serviceRequestFinished(QNetworkReply * reply);
};

class BaseAPIFactory
{
public:
    static BaseAPI * instance(QObject *parent = nullptr)
    {
        return new BaseAPI(parent);;
    }
};



#endif // BASEAPI_H
