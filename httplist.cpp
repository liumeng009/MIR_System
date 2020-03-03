#include "httplist.h"
#include "table.h"      //只是在cpp文件中用到的，不用写在头文件，避免重复包含


HttpList::HttpList(QObject *parent) : QObject(parent)
{
    connect(&getMission,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(GetMissionReply(QNetworkReply *)));

    connect(&postMissionQueue,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(PostMissionQueueReply(QNetworkReply *)));

    connect(&getStatus,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(GetStatusReply(QNetworkReply *)));

    connect(&postStatus,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(PostStatusReply(QNetworkReply *)));

    connect(&getPosition,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(GetPositionReply(QNetworkReply *)));

    connect(&getPositionXY,SIGNAL(UploadReply(QNetworkReply * )),
            this,SLOT(GetPositionXYReply(QNetworkReply *)));

    demoTimer = new QTimer(this);
    connect(demoTimer,SIGNAL(timeout()),this,SLOT(UpdateRobotStatus()));
    demoTimer->start(5000); /*周期5s*/

    positionTimer = new QTimer(this);
    connect(positionTimer,SIGNAL(timeout()),this,SLOT(PositonUpdate()));
    PositonUpdate();
    positionTimer->start(40000); /*周期40s*/

    executeTimer = new QTimer(this);
    connect(executeTimer,SIGNAL(timeout()),this,SLOT(TmrCallBack()));
    connect(executeTimer,SIGNAL(timeout()),this,SLOT(MirExceptionHandle()));
    executeTimer->start(4000); /*周期4s*/

}
/*机器人列表中的项目是由机器人配置表决定的
然后每个机器人会周期性地完成以下操作
1.getmission getstatus getposition获取到每个机器人上的配置信息
针对性地完成以下请求，请求先写好，然后根据标志位request决定
1.postmissionqueue 为了执行一次任务
2.poststatus 为了让机器人暂停启动充电
注：机器人表的主键是url，reply要根据url填写进机器人表中，每5s执行一次
*/
void HttpList::UpdateRobotStatus()
{
    static int counter;
    QList <BoxItem>::iterator itBox;
    QList <ExpressItem>::iterator itExpress;

    //先进性容错处理 读取机器人列表的数目
    if(GetBoxList()->size() == 0)   //如果没有机器人，那么就不进行处理
        return;

    //先处理周期性的读取任务
    //遍历然后一一请求
    for(itBox = GetBoxList()->begin(); itBox != GetBoxList()->end(); itBox++)
    {
        //getstatus
        GetStatusFunc(itBox->url);
    }

    if(++counter > 15)
    {
        counter = 0;
        for(itBox = GetBoxList()->begin(); itBox != GetBoxList()->end(); itBox++)
        {
            //getmission
            GetMissionFunc(itBox->url);
            //getPosition
            GetPositionFunc(itBox->url);
        }
    }

}
void HttpList::PositonUpdate()
{
    QList <BoxItem>::iterator it;
    QList <POSITION_INFO>::iterator pIt;

    /*容错*/
    if(GetBoxList()->size() == 0)
        return;

    //遍历一遍socketlist
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->casheGuidList->size() != 0)
        {
            //针对每个可用的socketlist 根据他的地址表guid和socket的url，更新“病区名”
            for(pIt = it->casheGuidList->begin(); pIt != it->casheGuidList->end(); pIt++)
            {
                //发送http Get请求，获取到名字+坐标后，再根据名字，更新坐标
                GetPositionXYFunc(it->url,pIt->guid);
            }
        }
    }

}

/*周期4s 执行任务列表中的任务，不阻塞*/
void HttpList::TmrCallBack()
{
    ExpressItem dataTask;
    QList <BoxItem>::iterator itBox;
    QList <ExpressItem>::iterator itExpress;
    QList <POSITION_INFO>::iterator itPosition;

    //再处理每个项的任务请求
    //遍历然后一一请求 1 发车 2 叫车
    /*容错*/
    if(GetBoxList()->size() == 0)
        return;

    /*查找socket的id是不是在箱体配置表中*/
    for(itBox = GetBoxList()->begin(); itBox != GetBoxList()->end(); itBox++)
    {
        //如果任务列表不空
        if(itBox->expressList->size() != 0)
        {
            //发车 ： 遍历任务列表id 结合病区表 得到病区名
            if(itBox->httpRequest == 1)
            {
                for(itExpress = itBox->expressList->begin(); itExpress != itBox->expressList->end(); itExpress++)
                {
                    //第一个未到达/正在前往（容错）的病区任务
                    if(itExpress->alreadyReached == 0 || itExpress->alreadyReached == 2)
                    {
                        //找到了可以发车的任务
                        if(itBox->execCounter == 0)
                            itBox->execCounter = 1;

                        qDebug()<<"发车"<<itExpress->name;
                        qDebug()<<"发车"<<itBox->url;
                        qDebug()<<"发车mission"<<itBox->missionIdSelected;
                        qDebug()<<"发车guid"<<itExpress->guid;
                        qDebug()<<"执行"<<itBox->execCounter;

                        //先清空可能存在的垃圾任务

//                        if(itBox->execCounter == 1)
//                        {
//                           DeleteQueueFunc(itBox->url);
//                           itBox->execCounter = 2;  //接力棒
//                        }

//                        if(itBox->execCounter == 2)
//                        {
                            /*putStatus (ready)*/
                            PostStatusFunc(itBox->url,
                                           "3");
//                           itBox->execCounter = 3;  //接力棒
//                        }

//                        if(itBox->execCounter == 3)
//                        {
                            /*postMissionQueue 这里需要反馈 后续补充*/
                            PostMissionQueueFunc(itBox->url,
                                                 "",
                                                 itBox->missionIdSelected,
                                                 "",
                                                 itExpress->guid,
                                                 1.0);
                            itBox->execCounter = 0;  //无动作

                           /*即将到达*/
                           itExpress->alreadyReached = 2;

                           /*真正执行后，要清空请求*/
                           itBox->httpRequest = 0;
                       //  }

                        break;
                    }
                }

            }
        }
        else    /*任务列表为空*/
        {
            if(itBox->httpRequest == 1)    /*无任务的情况下，并且httpRequest == 1,要清零请求，防止干扰下次任务*/
            {
                itBox->httpRequest == 0;
            }
        }

        /*叫车 不需要必须有任务*/
        if(itBox->httpRequest == 2)
        {
            qDebug()<<"静配中心添加";

            /*添加静配中心任务*/
            dataTask.taskBingquId = 0; //约定
            dataTask.alreadyReached = 0;
            dataTask.distance = 0.0;
            dataTask.guid = "";
            dataTask.map = "";
            dataTask.name = "静配中心";      //不用查表，直接赋值
            dataTask.range = 0;
            dataTask.url = "";
            dataTask.X = 0.0;
            dataTask.Y = 0.0;
            dataTask.timeout = 0;
            itBox->expressList->append(dataTask);

            //发车
            itBox->httpRequest = 1;
        }
    }
}
void HttpList::MirExceptionHandle()
{
    QList <BoxItem>::iterator itBox;

    /*容错*/
    if(GetBoxList()->size() == 0)
        return;

    /*查找socket的id是不是在箱体配置表中*/
    for(itBox = GetBoxList()->begin(); itBox != GetBoxList()->end(); itBox++)
    {
        /*如果当前状态不正常，那么就变成ready*/
        if(itBox->stateId1 == "10" || itBox->stateId1 == "12")
        {
            /*putStatus (ready)*/
            PostStatusFunc(itBox->url,
                           "3");
        }
    }
}

/*以下的操作，是在知道url的前提下，有周期的，有事件的操作*/
/*他们是Get Post Put基础操作*/
/*在他们的基础上，会产生一些事务性的状态变量*/
void HttpList::PostMissionFunc(const QString url,
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

    postMission.post(url+"missions", dataArray);
}
void HttpList::PostMissionReply(QNetworkReply *reply)
{
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();

    qDebug()<<reply->url().toString(); //谁回应的
    //targetUrl.remove("missions");  //去除多余类型描述

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

void HttpList::GetMissionFunc(const QString url)
{
    getMission.get(url + "missions");   
}
void HttpList::GetMissionReply(QNetworkReply *reply)
{
    QList <BoxItem>::iterator it;
    QString targetUrl;
    int socketListSize,findItFlag;
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray resultDataArray;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("missions");  //去除多余类型描述

    /*容错*/
    socketListSize = GetBoxList()->size();
    if(socketListSize == 0)
    {
        reply->deleteLater();
        return;
    }
    /*查找此url是否在robotlist列表中*/
    findItFlag = 0;
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->url == targetUrl)
        {
            findItFlag = 1; //对应值就是counter
            break;
        }
    }
    if(findItFlag == 0) //如果没有找到，说明数据无效
    {
        reply->deleteLater();
        return;
    }

    //如果已经找到。。
    MISSION_INFO missionInfo;   /*用于收集所有mission信息*/
    //清楚原来的列表，重新获得一次全部地址
    it->missionGuidList->clear();

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
                it->missionGuidList->append(missionInfo);
            }

        }
    }
    reply->deleteLater();
}

void HttpList::PostMissionQueueFunc(const QString url,
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

    postMissionQueue.post(url + "mission_queue", dataArray);
}
void HttpList::DeleteQueueFunc(const QString url)
{
    deleteQueue.deleteIt(url + "mission_queue");
}
void HttpList::GetMissionQueueFunc(const QString url)
{
    getMissionQueue.get(url);
}

void HttpList::PostMissionQueueReply(QNetworkReply *reply)
{
    reply->deleteLater();
}
void HttpList::GetMissionQueueReply(QNetworkReply *reply)
{
    reply->deleteLater();
}

void HttpList::GetStatusFunc(const QString url)
{
    getStatus.get(url + "status");
}
void HttpList::GetStatusReply(QNetworkReply *reply)
{
    return;
    QList <BoxItem>::iterator it;
    QString targetUrl;
    int robotListSize,counter,findItFlag;
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray tempPosition;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("status");  //去除多余类型描述

    /*容错*/
    robotListSize = GetBoxList()->size();
    if(robotListSize == 0)
    {
        reply->deleteLater();
        return;
    }
    /*查找此url是否在robotlist列表中*/
    findItFlag = 0;
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->url == targetUrl)
        {
            findItFlag = 1; //对应值就是counter
            break;
        }
    }
    if(findItFlag == 0) //如果没有找到，说明数据无效
    {
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
                        tempPosition += b.toVariant().toString();
                        it->pos_x = b.toDouble();
                    }
                    if(a.contains("y"))
                    {
                        QJsonValue b = a.value("y");
                        tempPosition += b.toVariant().toString();
                        it->pos_y = b.toDouble();
                    }
                    //it->position1 = tempPosition;
                }
            }
            /*其他字段如上...*/
            if (object.contains("state_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_id");  // 获取指定 key 对应的 value
                //if (value.isString())// 判断 value 是否为字符串
                //{
                    QString strName = value.toVariant().toString();  // 将 value 转化为字符串
                    it->stateId1 = strName;
                //}
            }
            if (object.contains("state_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    it->stateText1 = strName;
                }
            }
            if (object.contains("mission_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("mission_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    //httpSettings.stateText1 = strName;
                    it->missionText = strName;
                    qDebug()<<"missionText:"<<strName;

                    if(strName.startsWith('w',Qt::CaseInsensitive))
                    {
                        it->missionText = strName;
                        it->missionState = "waiting";
                        //it->targetBingqu保存上一状态
                    }
                    else if(strName.startsWith('m',Qt::CaseInsensitive))
                    {
                        QStringList result;
                        it->missionText = strName;
                        it->missionState = "working";
                        result = it->missionText.split(QRegExp("[']"));
                        it->targetBingqu = result[1];
                    }
                    qDebug()<<"missState:"<<it->missionState;
                    qDebug()<<"missionBingqu:"<<it->targetBingqu;

                }
            }
            if (object.contains("map_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("map_id");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    //qDebug()<<"map_id:"<<strName;
                    it->mapId = strName;
                }
            }
        }
    }

    reply->deleteLater();
}

void HttpList::PostStatusFunc(const QString url,
                              const QString state_id)
{
    //json to string
    QJsonObject json;
    json.insert("state_id", state_id.toInt());

    QJsonDocument document;
    document.setObject(json);
    QByteArray dataArray = document.toJson(QJsonDocument::Compact);
    //json to string

    postStatus.put(url + "status", dataArray);
}

void HttpList::PostStatusReply(QNetworkReply *reply)
{
    QList <BoxItem>::iterator it;
    QString targetUrl;
    int socketListSize,counter,findItFlag;
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray tempPosition;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("status");  //去除多余类型描述

    /*容错*/
    socketListSize = GetBoxList()->size();
    if(socketListSize == 0)
    {
        reply->deleteLater();
        return;
    }
    /*查找此url是否在robotlist列表中*/
    findItFlag = 0;
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->url == targetUrl)
        {
            findItFlag = 1; //对应值就是counter
            break;
        }
    }
    if(findItFlag == 0) //如果没有找到，说明数据无效
    {
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
                    it->position2 = tempPosition;
                }
            }
            /*其他字段如上...*/
            if (object.contains("state_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_id");  // 获取指定 key 对应的 value
                //if (value.isString())// 判断 value 是否为字符串
                //{
                    QString strName = value.toString();  // 将 value 转化为字符串
                    it->stateId2 = strName;
                //}
            }
            if (object.contains("state_text"))
            {  // 包含指定的 key
                QJsonValue value = object.value("state_text");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串
                    it->stateText2 = strName;
                }
            }
            /*其他字段如上...*/

        }
    }

    reply->deleteLater();
}


void HttpList::GetPositionFunc(const QString url)
{
    getPosition.get(url + "positions");
}
void HttpList::GetPositionReply(QNetworkReply *reply)
{
    QList <BoxItem>::iterator it;
    QString targetUrl;
    int socketListSize,counter,findItFlag;
    /*处理reply的json*/
    QByteArray dataArray = reply->readAll();
    QByteArray resultDataArray;
    targetUrl = reply->url().toString(); //哪个url回应的
    targetUrl.remove("positions");  //去除多余类型描述

    /*容错*/
    socketListSize = GetBoxList()->size();
    if(socketListSize == 0)
    {
        reply->deleteLater();
        return;
    }
    /*查找此url是否在SocketList列表中*/
    findItFlag = 0;
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->url == targetUrl)
        {
            findItFlag = 1; //对应值就是counter
            break;
        }
    }
    if(findItFlag == 0) //如果没有找到，说明数据无效
    {
        reply->deleteLater();
        return;
    }

    POSITION_INFO positionInfo; //用来收集所有位置信息
    //把此链接的地址列表先清除，此时已经收到了reply，不用担心数据会被别的模块采用
    it->casheGuidList->clear();

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
                            positionInfo.guid = jValue.toString();
                        }
                    }

                    if(jobject.contains("map"))
                    {
                        QJsonValue jValue = jobject.value("map");
                        if(jValue.isString())
                        {
                            resultDataArray += "  map:";
                            resultDataArray += jValue.toString();
                            positionInfo.map = jValue.toString().remove("/v2.0.0/maps/");
                        }
                    }

                    if(jobject.contains("url"))
                    {
                        QJsonValue jValue = jobject.value("url");
                        if(jValue.isString())
                        {
                            resultDataArray += "  url:";
                            resultDataArray += jValue.toString();
                            positionInfo.url = jValue.toString();
                        }
                    }

                    resultDataArray += '\n';
                }
                it->casheGuidList->append(positionInfo);
            }

        }
    }
    reply->deleteLater();
}

void HttpList::GetPositionXYFunc(const QString url,const QString guid)
{
    getPositionXY.get(url + "positions/"+guid);
}
void HttpList::GetPositionXYReply(QNetworkReply *reply)
{
    QList <BoxItem>::iterator it;
    QList <POSITION_INFO> * pPos;
    QList <POSITION_INFO>::iterator itPos;
    QStringList d;
    QString targetUrl;  /*确定是哪一个url*/
    QString targetGuid; /*确定是哪一个guid*/
    int socketListSize,findItFlag;

    /*处理reply*/
    QByteArray dataArray = reply->readAll();
    targetUrl = reply->url().toString(); //哪个url回应的
    d= targetUrl.split(QRegExp("[/]"));
    targetUrl = d[0]+"/"+d[1]+"/"+d[2]+"/"+d[3]+"/"+d[4]+"/";
    targetGuid = d[6];
    //qDebug()<<targetUrl<<"|"<<targetGuid;

    /*容错*/
    socketListSize = GetBoxList()->size();
    if(socketListSize == 0)
    {
        reply->deleteLater();
        return;
    }
    /*查找此url是否在socketList列表中*/
    findItFlag = 0;
    for(it = GetBoxList()->begin(); it != GetBoxList()->end(); it++)
    {
        if(it->url == targetUrl)
        {
            findItFlag = 1; //对应值就是counter
            //找到对应的地址列表
            pPos = it->casheGuidList;
            break;
        }
    }
    if(findItFlag == 0) //如果没有找到，说明数据无效
    {
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

            if (object.contains("name"))
            {  // 包含指定的 key
                QJsonValue value = object.value("name");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串

                    //确定GUID，然后更新name
                    if(pPos != NULL)
                    {
                        if(pPos->size() != 0)
                        {
                            for(itPos = pPos->begin(); itPos != pPos->end(); itPos++)
                            {
                                if(itPos->guid == targetGuid)
                                {
                                    itPos->name = strName;
                                }
                            }
                        }
                    }

                }
            }
            if (object.contains("map_id"))
            {  // 包含指定的 key
                QJsonValue value = object.value("map_id");  // 获取指定 key 对应的 value
                if (value.isString())// 判断 value 是否为字符串
                {
                    QString strName = value.toString();  // 将 value 转化为字符串

                    //确定GUID，然后更新name
                    if(pPos != NULL)
                    {
                        if(pPos->size() != 0)
                        {
                            for(itPos = pPos->begin(); itPos != pPos->end(); itPos++)
                            {
                                if(itPos->guid == targetGuid)
                                {
                                    itPos->map = strName;
                                }
                            }
                        }
                    }

                }
            }

            if (object.contains("pos_x"))
            {  // 包含指定的 key
                QJsonValue value = object.value("pos_x");  // 获取指定 key 对应的 value
                if (value.isDouble())// 判断 value 是否double
                {
                    double num = value.toDouble();  // 将 value 转化为字符串
                    //确定GUID，然后更新name
                    if(pPos != NULL)
                    {
                        if(pPos->size() != 0)
                        {
                            for(itPos = pPos->begin(); itPos != pPos->end(); itPos++)
                            {
                                if(itPos->guid == targetGuid)
                                {
                                    itPos->X = num;
                                }
                            }
                        }
                    }
                }
            }

            if (object.contains("pos_y"))
            {  // 包含指定的 key
                QJsonValue value = object.value("pos_y");  // 获取指定 key 对应的 value
                if (value.isDouble())// 判断 value 是否double
                {
                    double num = value.toDouble();  // 将 value 转化为字符串
                    //确定GUID，然后更新name
                    if(pPos != NULL)
                    {
                        if(pPos->size() != 0)
                        {
                            for(itPos = pPos->begin(); itPos != pPos->end(); itPos++)
                            {
                                if(itPos->guid == targetGuid)
                                {
                                    itPos->Y = num;
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    reply->deleteLater();
}
