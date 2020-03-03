#include "MIRCmdCON.h"
#include <QDebug>
/*
 * GetMission           1
 * PostMissionQueue     2
 * DeleteQueue          3
 * GetStatus            4
 * PostStatus(abort)    5
 * PostStatus(ready)    6
 * GetPosition          7
 * GetPositionXY(name)  8
*/

/* MIR op
 * 1/Starting
 * 2/Shutting down
 * 3/Ready
 * 4/Pause
 * 5/Executing
 * 6/Aborted
 * 7/Completed
 * 8/Docked
 * 9/Docking
 * 10/Emergency Stop
 * 11/Manual Control
 * 12/Error
*/

MIRCmdCON::MIRCmdCON(QObject *parent, MIRSchedule *mirSchedule) : QObject(parent)
{
    this->mirSchedule = mirSchedule;
}

void MIRCmdCON::AbortMirExecuting()
{
    /*pause*/
    PostStatusFunc("4");
}

void MIRCmdCON::StartMirExecuting()
{
    /*pause*/
    PostStatusFunc("3");
}
void MIRCmdCON::DeleteMirQueue()
{
    DeleteQueueFunc();
}
void MIRCmdCON::GetMirState()
{
    GetStatusFunc();
}

void MIRCmdCON::ExecTask(QString dest)
{
    //to do ....dest->guid
    mirSchedule->SetMissionGuid();  //重新计算一下missionGuid
    QString positionGuid = mirSchedule->GetPositonGuid(dest);

    //地名过滤...

    //目标确认
    this->dest = dest;

    /*dest区分*/
    if(dest == QStringLiteral("充电站")) /*停靠*/
    {
        PostMissionQueueFunc(QString(QStringLiteral("message")),
                             mirSchedule->chargeMissionIdSelected,
                             QString(QStringLiteral("message")),
                             positionGuid,
                             1.0);
    }
    else /*普通移动*/
    {
        PostMissionQueueFunc(QString(QStringLiteral("message")),
                             mirSchedule->missionIdSelected,
                             QString(QStringLiteral("message")),
                             positionGuid,
                             1.0);
    }
}
void MIRCmdCON::GetMissionList()
{
    GetMissionFunc();
}
void MIRCmdCON::GetPositionList()
{
    GetPositionFunc();
}
void MIRCmdCON::ClearError()
{
    PostStatusClearErrorFunc();
}


/**********************以上是对外API******************************/

int MIRCmdCON::ReplayStatusCode(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    return statusCode;
}
/*以下的操作，是在知道url的前提下的操作*/
/*返回结果各自处理*/
void MIRCmdCON::PostMissionFunc(const QString url,
                               const QString groupID,
                               const QString name)
{

}

void MIRCmdCON::PostMissionReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();

    qDebug()<<reply->url().toString(); //谁回应的
    //targetUrl.remove("missions");  //去除多余类型描述

    qDebug()<<"status code"<<ReplayStatusCode(reply);

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())// JSON 文档为对象
        {
            QJsonObject object = doucment.object();  // 转化为对象

            /*字段提取*/
            if (object.contains("name"))
            {  // 包含指定的 key
                QJsonValue value = object.value("name");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    qDebug() << "Name : " << strName;
                }
            }
            /*其他字段如上...*/

        }
    }

    reply->deleteLater();
}

void MIRCmdCON::GetMissionFunc()
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(GetMissionReply(QNetworkReply*)));

    httpCON->GetMissionFunc(mirSchedule->URL);   //httpCON自动释放

}
void MIRCmdCON::GetMissionReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray resultDataArray;

    QString targetUrl;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("missions");  //去除多余类型描述

    /*容错*/
    qDebug()<<"status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(1,false,reply->errorString());
        reply->deleteLater();
        return;
    }

    MISSION_INFO missionInfo;   /*用于收集所有mission信息*/
    //清除原来的列表，重新获得一次全部地址
    mirSchedule->missionGuidList.clear();

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isArray())// JSON 文档为数组
        {
            QJsonArray jsonArray = doucment.array();  // 转化为数组
            /*字段提取*/
            for (int i = 0; i < jsonArray.count();i++)
            {
                QJsonValue value = jsonArray[i];

                if(value.isObject())
                {
                    QJsonObject jobject = value.toObject();
                    if(jobject.contains("guid"))
                    {
                        QJsonValue jValue = jobject.value("guid");
                        if(jValue.isString())
                        {
                            resultDataArray += "  guid:";
                            resultDataArray += jValue.toString();
                            missionInfo.guid = jValue.toString();

                        }
                    }

                    if(jobject.contains("name"))
                    {
                        QJsonValue jValue = jobject.value("name");
                        if(jValue.isString())
                        {
                            resultDataArray += "  name:";
                            resultDataArray += jValue.toString();
                            missionInfo.name = jValue.toString();
                        }
                    }

                    if(jobject.contains("url"))
                    {
                        QJsonValue jValue = jobject.value("url");
                        if(jValue.isString())
                        {
                            resultDataArray += "  url:";
                            resultDataArray += jValue.toString();
                            missionInfo.url = jValue.toString();
                        }
                    }

                    resultDataArray += '\n';
                }
                //采集完一项以后，就插入到列表中，前提列表已经被清除
                mirSchedule->missionGuidList.append(missionInfo);
            }

        }
    }

    emit MIRCmdCONFinished(1,true,"GetMissionOK");
    reply->deleteLater();
}

void MIRCmdCON::PostMissionQueueFunc(const QString message,
                                    const QString missionID,
                                    const QString array1,
                                    const QString array2,
                                    const float priority)
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(PostMissionQueueReply(QNetworkReply*)));

    httpCON->PostMissionQueueFunc(mirSchedule->URL,
                                  message,
                                  missionID,
                                  array1,
                                  array2,
                                  priority);

}
void MIRCmdCON::PostMissionQueueReply(QNetworkReply *reply)
{
    qDebug()<<"status code"<<ReplayStatusCode(reply);

    if(ReplayStatusCode(reply) == 200 || ReplayStatusCode(reply) == 201)
    {
        emit MIRCmdCONFinished(2,true,QStringLiteral("PostMissionQueueOK"));
    }
    else
    {

        emit MIRCmdCONFinished(2,false,reply->errorString());
    }

    reply->deleteLater();
}

void MIRCmdCON::DeleteQueueFunc()
{
    //baseAPi.deleteIt(url + "mission_queue");
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(DeleteQueueFuncReply(QNetworkReply*)));

    httpCON->DeleteQueueFunc(mirSchedule->URL);
}
void MIRCmdCON::DeleteQueueFuncReply(QNetworkReply *reply)
{
    qDebug()<<"status code"<<ReplayStatusCode(reply);

    if(ReplayStatusCode(reply) == 204)
        emit MIRCmdCONFinished(3,true,"DeleteQueueOK");
    else
    {
        emit MIRCmdCONFinished(3,false,reply->errorString());
        reply->deleteLater();
        return;
    }
    reply->deleteLater();
}


void MIRCmdCON::GetMissionQueueFunc(const QString url)
{
    //baseAPi.get(url);
}
void MIRCmdCON::GetMissionQueueReply(QNetworkReply *reply)
{
    reply->deleteLater();
}

void MIRCmdCON::GetStatusFunc()
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(GetStatusReply(QNetworkReply*)));

    httpCON->GetStatusFunc(mirSchedule->URL);
}
void MIRCmdCON::GetStatusReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray tempPosition;

    QString targetUrl;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("status");  //去除多余类型描述

    /*容错*/
    qDebug()<<"status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(4,false,QStringLiteral("GetStatusFailed") + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())// JSON 文档为对象
        {
            QJsonObject object = doucment.object();  // 转化为对象

            /*字段提取*/
            if (object.contains("position"))
            {  // 包含指定的 key
                QJsonValue value = object.value("position");  // 获取指定 key 对应的 value
                if (value.isObject())// 判断 value 是否为对象
                {
                    QJsonObject a = value.toObject();

                    if(a.contains("orientation"))
                    {
                        QJsonValue b = a.value("orientation");
                        tempPosition += b.toVariant().toString();
                    }
                    if(a.contains("x"))
                    {
                        QJsonValue b = a.value("x");
                        mirSchedule->state.pos_x = b.toDouble();
                    }
                    if(a.contains("y"))
                    {
                        QJsonValue b = a.value("y");
                        mirSchedule->state.pos_y = b.toDouble();
                    }
                }
            }

            if (object.contains("state_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_id");  // 获取指定 key 对应的 value
                QString strName = value.toVariant().toString();  // 将 value 转化为字符串
                mirSchedule->state.stateId = strName;

            }
            if (object.contains("state_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    mirSchedule->state.stateText = strName;
                }
            }
            if (object.contains("battery_percentage"))
            {  // 包含指定的 key
                QJsonValue value = object.value("battery_percentage");  // 获取指定 key 对应的 value
                if (value.isDouble())// 判断 value 是否为字符串
                {
                    mirSchedule->state.battery_percentage = value.toDouble(100.0);
                }
            }
            if (object.contains("battery_time_remaining"))
            {  // 包含指定的 key
                QJsonValue value = object.value("battery_time_remaining");  // 获取指定 key 对应的 value
                mirSchedule->state.battery_time_remaining = value.toInt(100);
            }

            if (object.contains("mission_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("mission_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    mirSchedule->state.missionText = strName;
                    /*由missionText-》missionState(不怎么用)和MIRTargetBingqu*/
                    if(strName.startsWith('w',Qt::CaseInsensitive))
                    {
                        mirSchedule->state.missionState = "waiting";
                    }
                    else if(strName.startsWith('m',Qt::CaseInsensitive))
                    {
                        QStringList result;

                        mirSchedule->state.missionState = "working";
                        result = mirSchedule->state.missionText.split(QRegExp("[']"));

                        if(result.size() >= 2)
                            mirSchedule->state.MIRTargetBingqu = result[1];
                    }
                    else if(strName.startsWith('charging',Qt::CaseInsensitive))
                    {
                        mirSchedule->state.missionState = "charging";
                    }
                }
            }
            if (object.contains("map_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("map_id");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    //qDebug()<<"map_id:"<<strName;
                    mirSchedule->state.mapId = strName;
                }
            }
        }
    }


    emit MIRCmdCONFinished(4,true,QStringLiteral("GetStatusOK"));
    reply->deleteLater();
}

void MIRCmdCON::PostStatusFunc(const QString state_id)
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(PostStatusReply(QNetworkReply*)));

    httpCON->PostStatusFunc(mirSchedule->URL,state_id); //httpCON自动释放
}
void MIRCmdCON::PostStatusClearErrorFunc()
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(PostStatusClearErrorReply(QNetworkReply*)));

    httpCON->PostStatusClearErrorFunc(mirSchedule->URL); //httpCON自动释放
}

void MIRCmdCON::PostStatusReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray tempPosition;

    QString targetUrl;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("status");  //去除多余类型描述

    /*容错*/
    qDebug()<<"status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(5,false,QStringLiteral("PostStatusfailed") + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())// JSON 文档为对象
        {
            QJsonObject object = doucment.object();  // 转化为对象

            /*字段提取*/
            if (object.contains("position"))
            {  // 包含指定的 key
                QJsonValue value = object.value("position");  // 获取指定 key 对应的 value
                if (value.isObject())// 判断 value 是否为对象
                {
                    QJsonObject a = value.toObject();

                    if(a.contains("orientation"))
                    {
                        QJsonValue b = a.value("orientation");
                        tempPosition += b.toVariant().toString();
                    }
                    if(a.contains("X"))
                    {
                        QJsonValue b = a.value("X");
                        tempPosition += b.toVariant().toString();
                    }
                    if(a.contains("Y"))
                    {
                        QJsonValue b = a.value("Y");
                        tempPosition += b.toVariant().toString();
                    }
                }
            }
            /*其他字段如上...*/
            if (object.contains("state_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_id");  // 获取指定 key 对应的 value
                //if (value.isString())// 判断 value 是否为字符串
                //{
                    QString strName = value.toString();  // 将 value 转化为字符串
                    mirSchedule->state.stateId = strName;
                //}
            }
            if (object.contains("state_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    mirSchedule->state.stateText = strName;
                }
            }
            /*其他字段如上...*/

        }
    }

    emit MIRCmdCONFinished(5,true,QStringLiteral("PostStatusOK"));
    reply->deleteLater();
}

void MIRCmdCON::PostStatusClearErrorReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray tempPosition;

    QString targetUrl;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("status");  //去除多余类型描述

    /*容错*/
    qDebug()<<"clear error status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(8,false,QStringLiteral("PostStatusClearErrorfailed") + reply->errorString());
        reply->deleteLater();
        return;
    }

    emit MIRCmdCONFinished(8,true,QStringLiteral("PostStatusClearErrorOK"));
    reply->deleteLater();
}


void MIRCmdCON::GetPositionFunc()
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(GetPositionReply(QNetworkReply*)));

    httpCON->GetPositionFunc(mirSchedule->URL); //httpCON自动释放
}
void MIRCmdCON::GetPositionReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray resultDataArray;

    QString targetUrl;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("positions");  //去除多余类型描述

    /*容错*/
    qDebug()<<"status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(7,false,QStringLiteral("GetPositionsOK") + reply->errorString());
        reply->deleteLater();
        return;
    }

    POSITION_INFO positionInfo; //用来收集所有位置信息
    mirSchedule->positonGuidList.clear();

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isArray())// JSON 文档为数组
        {
            QJsonArray jsonArray = doucment.array();  // 转化为数组
            /*字段提取*/
            foreach (QJsonValue value, jsonArray)
            {
                if(value.isObject())
                {
                    QJsonObject jobject = value.toObject();
                    if(jobject.contains("guid"))
                    {
                        QJsonValue jValue = jobject.value("guid");
                        if(jValue.isString())
                        {
                            positionInfo.guid = jValue.toString();
                        }
                    }
                    /*可以获取name，但是接下来要判断xy是否有效，所以不在这里获取*/
//                    if(jobject.contains("name"))
//                    {
//                        QJsonValue jValue = jobject.value("name");
//                        if(jValue.isString())
//                        {
//                            resultDataArray += "  guid:";
//                            resultDataArray += jValue.toString();
//                            positionInfo.name = jValue.toString();
//                        }
//                    }

                    if(jobject.contains("map"))
                    {
                        QJsonValue jValue = jobject.value("map");
                        if(jValue.isString())
                        {
                            positionInfo.map = jValue.toString().remove("/v2.0.0/maps/");
                        }
                    }

                    if(jobject.contains("url"))
                    {
                        QJsonValue jValue = jobject.value("url");
                        if(jValue.isString())
                        {
                            positionInfo.url = jValue.toString();
                        }
                    }
                }
                mirSchedule->positonGuidList.append(positionInfo);
            }
        }
    }

    emit MIRCmdCONFinished(7,true,QStringLiteral("GetPositionsOK"));
    reply->deleteLater();

}
/*获得某一个GUID的XY坐标*/
void MIRCmdCON::GetPositionXYFunc(const QString guid)
{
    HttpCON * httpCON = HttpCONFactory::instance(this);

    connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
            this,SLOT(GetPositionXYReply(QNetworkReply*)));

    httpCON->GetPositionXYFunc(mirSchedule->URL,guid); //httpCON自动释放
}
void MIRCmdCON::GetPositionXYReply(QNetworkReply *reply)
{
    QList <POSITION_INFO>::iterator itPos;
    QStringList d;
    QString targetUrl;  /*确定是哪一个url*/
    QString targetGuid; /*确定是哪一个guid*/

    /*处理reply*/
    QByteArray dataArray = reply->readAll();
    targetUrl = reply->url().toString(); //哪个url回应的
    d= targetUrl.split(QRegExp("[/]"));
    targetUrl = d[0]+"/"+d[1]+"/"+d[2]+"/"+d[3]+"/"+d[4]+"/";
    targetGuid = d[6];
    //qDebug()<<targetUrl<<"|"<<targetGuid;

    /*容错*/
    qDebug()<<"pos status code"<<ReplayStatusCode(reply);
    if(ReplayStatusCode(reply) != 200)
    {
        emit MIRCmdCONFinished(8,false,QStringLiteral("GetPositions Failed") + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonParseError jsonError;  //error
    QJsonDocument doucment = QJsonDocument::fromJson(dataArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())// JSON 文档为对象
        {
            QJsonObject object = doucment.object();  // 转化为对象

            /*字段提取*/
            if (object.contains("guid"))
            {  // 包含指定的 key
                QJsonValue value = object.value("guid");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    targetGuid = value.toString();  //接下来针对此guid操作
                }
            }

            //定位到要更新的地址项
            if(!mirSchedule->positonGuidList.isEmpty())
            {
                int findFlag = 0;
                for(itPos = mirSchedule->positonGuidList.begin();
                    itPos != mirSchedule->positonGuidList.end();
                    itPos++)
                {
                    if(itPos->guid == targetGuid)
                    {
                        findFlag = 1;
                        break;
                    }
                }
                //没有更新的目标
                if(findFlag == 0)
                {
                    emit MIRCmdCONFinished(8,false,QStringLiteral("GetPositionsOK not find"));
                    reply->deleteLater();
                    return;
                }
            }
            else
            {
                //没有更新目标
                emit MIRCmdCONFinished(8,false,QStringLiteral("GetPositionsOK not find"));
                reply->deleteLater();
                return;
            }

            //填入数据
            if (object.contains("name"))
            {  // 包含指定的 key
                QJsonValue value = object.value("name");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    //更新name
                    itPos->name = strName;
                }
            }
            if (object.contains("map_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("map_id");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    //更新mapId
                    itPos->map = strName;
                }
            }

            if (object.contains("pos_x"))
            {  // 包含指定的 key
                QJsonValue value = object.value("pos_x");  // 获取指定 key 对应的 value
                if (value.isDouble())// 判断 value 是否double
                {
                    double num = value.toDouble();  // 将 value 转化为字符串
                    //更新X
                    itPos->X = num;
                }
            }

            if (object.contains("pos_y"))
            {  // 包含指定的 key
                QJsonValue value = object.value("pos_y");  // 获取指定 key 对应的 value
                if (value.isDouble())// 判断 value 是否double
                {
                    double num = value.toDouble();  // 将 value 转化为字符串
                    //更新Y
                    itPos->Y = num;
                }
            }
        }
    }

    emit MIRCmdCONFinished(8,true,QStringLiteral("GetPositionsOK"));
    reply->deleteLater();
}

void MIRCmdCON::GetAllPositionXYFunc()
{
    int counter = 0;

    foreach (POSITION_INFO item, mirSchedule->positonGuidList) {

        counter++;  /*0123456789排序*/

        HttpCON * httpCON = HttpCONFactory::instance(this);

        connect(httpCON,SIGNAL(UploadReply(QNetworkReply*)),
                this,SLOT(GetPositionXYReply(QNetworkReply*)));

        QTimer * timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout,
                this,[=](){
            httpCON->GetPositionXYFunc(mirSchedule->URL,item.guid); //httpCON自动释放
        });
        timer->start(counter * 20);
    }
}
