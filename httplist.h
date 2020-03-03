#ifndef HTTPLIST_H
#define HTTPLIST_H

#include <QObject>
#include <QtGui>
#include "QNetworkReply"
#include <QTimer>
#include "QJsonObject"
#include "QJsonDocument"
#include "QByteArray"
#include "QJsonArray"

class HttpList : public QObject
{
    Q_OBJECT
public:
    explicit HttpList(QObject *parent = nullptr);

    QTimer * demoTimer;
    QTimer * positionTimer;
    QTimer * executeTimer;

    /*每一种类型的请求*/

    /*操作以上对象,供外部调用API*/
    void PostMissionFunc(const QString url,
                         const QString groupID,
                         const QString name);
    void GetMissionFunc(const QString url);
    void PostMissionQueueFunc(const QString url,
                              const QString message,
                              const QString missionID,
                              const QString array1,
                              const QString array2,
                              const float priority);
    void DeleteQueueFunc(const QString url);
    void GetMissionQueueFunc(const QString url);
    void GetStatusFunc(const QString url);
    void PostStatusFunc(const QString url,
                        const QString state_id);
    void GetPositionFunc(const QString url);
    void GetPositionXYFunc(const QString url,const QString guid);

signals:

public slots:
    /*ReplyProcee每当收到一个http response，那么就执行一次
    reply中含有url，可以定位到具体的机器人。
    具体有5中reply，对应这5个命令，因为每个请求的格式不同，参数不同需要单独处理
    1.首先解析reply，这个在接收函数中已经完成
    2.然后遍历robotlist，查找url相同的那个，将结果填入robotlist*/
    void PostMissionReply(QNetworkReply *reply);
    void GetMissionReply(QNetworkReply *reply);
    void PostMissionQueueReply(QNetworkReply *reply);
    void GetMissionQueueReply(QNetworkReply *reply);
    void GetStatusReply(QNetworkReply *reply);
    void PostStatusReply(QNetworkReply *reply);
    void GetPositionReply(QNetworkReply *reply);
    void GetPositionXYReply(QNetworkReply *reply);

    void TmrCallBack();
    void UpdateRobotStatus();
    void PositonUpdate();
    void MirExceptionHandle();
};

#endif // HTTPLIST_H
