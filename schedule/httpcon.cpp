#include "httpcon.h"

HttpCON::HttpCON(QObject *parent) : QObject(parent)
  ,baseAPi(BaseAPIFactory::instance(this))
{
    //返回值利用信号与槽回传
    connect(baseAPi,SIGNAL(UploadReply(QNetworkReply*)),
            this,SIGNAL(UploadReply(QNetworkReply*)));

    //传递完成后删除自己,同事删除子对象baseAPI,注意QNetworkReply的释放!
//    connect(baseAPi,SIGNAL(UploadReply(QNetworkReply*)),
//            this,SLOT(deleteLater()));
}

/*以下的操作，是在知道url的前提下，有周期的，有事件的操作*/
/*他们是Get Post Put基础操作*/
/*在他们的基础上，会产生一些事务性的状态变量*/
void HttpCON::PostMissionFunc(const QString url,
                               const QString groupID,
                               const QString name)
{
    //json to string
    QJsonObject json;
    json.insert("group_id", groupID);
    json.insert("name", name);

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);
    //json to string

    baseAPi->post(url+"missions", dataArray);
}
void HttpCON::GetMissionFunc(const QString url)
{
    baseAPi->get(url + "missions");
}
void HttpCON::PostMissionQueueFunc(const QString url,
                                    const QString message,
                                    const QString missionID,
                                    const QString array1,
                                    const QString array2,
                                    const float priority)
{
    //json to string
    QJsonObject json;
    json.insert("message", message);
    json.insert("mission_id", missionID);

    QJsonObject jsonPara1,jsonPara2;   //{..}为一对象，包含两个成员
    //jsonPara1.insert("input_name", "start_position");
    //jsonPara1.insert("value", array1);
    jsonPara2.insert("input_name", "end_position");
    jsonPara2.insert("value", array2);

    QJsonArray JsonArray;
    //JsonArray.push_back(jsonPara1);
    JsonArray.push_back(jsonPara2);

    json.insert("parameters",JsonArray);
    json.insert("priority", priority);

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);
    //json to string

    baseAPi->post(url + "mission_queue", dataArray);
}
void HttpCON::DeleteQueueFunc(const QString url)
{
    baseAPi->deleteIt(url + "mission_queue");
}
void HttpCON::GetMissionQueueFunc(const QString url)
{
    baseAPi->get(url);
}

void HttpCON::GetStatusFunc(const QString url)
{
    baseAPi->get(url + "status");
}
void HttpCON::PostStatusFunc(const QString url,
                              const QString state_id)
{
    //json to string
    QJsonObject json;
    json.insert("state_id", state_id.toInt());

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);
    //json to string

    baseAPi->put(url + "status", dataArray);
}
void HttpCON::PostStatusClearErrorFunc(const QString url)
{
    //json to string
    QJsonObject json;
    json.insert("clear_error", true);

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);
    //json to string

    baseAPi->put(url + "status", dataArray);
}
void HttpCON::GetPositionFunc(const QString url)
{
    baseAPi->get(url + "positions");
}
void HttpCON::GetPositionXYFunc(const QString url,const QString guid)
{
    baseAPi->get(url + "positions/"+guid);
}
