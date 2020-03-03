#ifndef HTTPCON_H
#define HTTPCON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include "baseapi.h"
#include "mirschedule.h"

/*      sche++++++>
 *                HttpCON++++++>
 *                              baseAPI
 *                HttpCON<-----
 *      sche<------
*/

class HttpCON : public QObject
{
    Q_OBJECT
public:
    explicit HttpCON(QObject *parent = nullptr);

    //依赖
    BaseAPI * baseAPi = nullptr;

    /*供外部调用API*/
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
    void PostStatusClearErrorFunc(const QString url);

    void GetPositionFunc(const QString url);
    void GetPositionXYFunc(const QString url,const QString guid);

signals:
    void UploadReply(QNetworkReply*);   //统一回复信号
public slots:

};

/*
 * 工厂说明：此类（即用即删）不建议作为父对象，BaseAPI除外
*/
class HttpCONFactory
{
public:
    static HttpCON * instance(QObject *parent = nullptr)
    {
        return new HttpCON(parent);;
    }
};

#endif // HTTPCON_H
