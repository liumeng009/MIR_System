#include "requestqueuecon.h"

RequestQueueCON::RequestQueueCON(QObject *parent,
                                 MIRSchedule * mirSchedule) : QObject(parent)
{
    this->mirSchedule = mirSchedule;
}
/*
 * 判断请求队列是否为空
*/
bool RequestQueueCON::IsQueueEmpty()
{
    return mirSchedule->requestList.isEmpty();
}
/*
 * 某请求是不是 <执行中>？
 *  但也可能请求压根就不存在，所以这里还要注意
*/
bool RequestQueueCON::IsRequestExecuting(QString bingqu)
{
    //遍历requestList state 0未执行 1执行中 2执行完
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state == 1)
            return true;
    }
    return false;
}
/*
 * 某请求是不是 <未执行>？
 *  但也可能请求压根就不存在，所以这里还要注意
*/
bool RequestQueueCON::IsRequestNotExecuting(QString bingqu)
{
    //遍历requestList state 0未执行 1执行中 2执行完
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state == 0)
            return true;
    }
    return false;
}
/*
 * open 的 getter方法 和下面有点重复
*/
bool RequestQueueCON::IsRequestOpened(QString bingqu)
{
    //遍历requestList state 0未执行 1执行中 2执行完
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->opened == 1 && item->state < 2)
            return true;
    }
    return false;
}

//集散地 IsRequestExecuting(”集散地“);
/*
 * 插入请求，如果size = 0；那和append没有区别
 * 如果size！= 0 ：那么默认集散地在最后，所以是从倒数第二个插入
 * （在这里有点取巧了，利用了insert的官网i<=0 == append）
*/
bool RequestQueueCON::InsertRequest(QString bingqu, QString requestContent)
{
    //倒数第二个插入
    mirSchedule->requestList.insert(mirSchedule->requestList.size()-1,
                CreateRequestNode(mirSchedule,bingqu,requestContent)
                );
    return true;
}
/*
 * 清空请求队列，删除对象+清空容器
*/
bool RequestQueueCON::RequestQueueClear()
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        item->deleteLater();
    }
    mirSchedule->requestList.clear();
}
/*
 * 插入初始请求：就是 （普通 + 集散） 的同时插入
*/
bool RequestQueueCON::InsertInitialRequest(QString bingqu , QString requestContent)
{
    //构建node + end node
    mirSchedule->requestList.append(
                CreateRequestNode(mirSchedule,bingqu,requestContent)
                );

    /*为了考虑更改，此时的集散地，最好用编号，从DB中进行查询, 后续更改*/
    mirSchedule->requestList.append(
                CreateRequestNode(mirSchedule,QStringLiteral("集散地"),QStringLiteral(""))
                );

    return true;
}
/*
 *  gocmd 的 setter 方法
*/
bool RequestQueueCON::SetRequestGoCmd(QString bingqu, int go)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            item->goCmd = go;
            return true;
        }
    }
    return false;
}
/*
 *  gocmd 的 getter 方法
*/
int  RequestQueueCON::GetRequestGoCmd(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
            return item->goCmd;
    }
    return 0;
}
/*
 *  设置某请求的open标志位
*/
bool RequestQueueCON::SetRequestOpenedFlag(QString bingqu, int opened)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            item->opened = opened;
            return true;
        }
    }
    return false;
}
/*
 *  获得某请求的open标志位
*/
int  RequestQueueCON::GetRequestOpenedFlag(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
            return item->opened;
    }
    return 0;
}
/*
 * 获得请求（bingqu）的类型，1普通 2材料 3集散 4充电（特殊对待，方案中进行补充）
*/
int RequestQueueCON::GetRequestType(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
            return item->type;
    }
    return -1;
}
/*
 * 获得请求内容，这里主要针对的是普通任务，也就是手术室申请任务
 * 获得的内容为字符串
*/
QString RequestQueueCON::GetRequestContent(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
            return item->requestContent;
    }
    return QStringLiteral("");
}

/*
 * 删除某请求，注意是直接删除，（此请求不是已完成请求）
*/
bool RequestQueueCON::DeleteRequest(QString bingqu)
{
    if(!mirSchedule->requestList.isEmpty())
    {
        for(int i = 0; i <= mirSchedule->requestList.size(); i++)
        {
            if(mirSchedule->requestList.at(i)->bingqu == bingqu
                    && mirSchedule->requestList.at(i)->state < 2)
            {
                mirSchedule->requestList.at(i)->deleteLater();  /*首先对象释放*/
                mirSchedule->requestList.removeAt(i);           /*然后改变容器*/
                return true;
            }
        }
    }
    return false;
}
/*
 * 删除某请求，并且置位后面的请求go=1
 * 注意：此函数
*/
bool RequestQueueCON::DeleteRequestWithGoCmdNext(QString bingqu)
{
    if(!mirSchedule->requestList.isEmpty()) /*请求队列有东西*/
    {
        for(int i = 0; i <= mirSchedule->requestList.size(); i++) /*遍历请求队列*/
        {
            if(mirSchedule->requestList.at(i)->bingqu == bingqu
                    && mirSchedule->requestList.at(i)->state < 2)   /*锁定当前请求*/
            {
                mirSchedule->requestList.at(i)->deleteLater();      /*首先释放对象*/
                mirSchedule->requestList.removeAt(i);               /*然后容器删除该请求*/
                //下一个是i了 先容错
                if(i <= mirSchedule->requestList.size() -1)
                {
                    for(int j = i; j < mirSchedule->requestList.size(); j++)  /*遍历后边的请求*/
                    {
                        if(mirSchedule->requestList.at(j)->state < 2)   /*保证未完成*/
                        {
                            /*仅仅紧接着的请求go=1，这之后的不管*/
                            mirSchedule->requestList.at(j)->goCmd = 1;
                            return true;    /*true表明，有请求go=1了*/
                        }
                    }
                }
            }
        }
    }
    return false;
}
/*
*   设置下一个请求为go
*   1.首先锁定当前的请求
*   2.判断后面是否还有请求
*   3.根据请求的执行状态，如果是未执行的，决定是否设置为go(仅仅一个)
*/
bool RequestQueueCON::SetGoCmdNext(QString bingqu)
{
    if(!mirSchedule->requestList.isEmpty())     /*有请求*/
    {
        for(int i = 0; i < mirSchedule->requestList.size(); i++)    /*遍历请求队列，寻找当前bingqu*/
        {
            if(mirSchedule->requestList.at(i)->bingqu == bingqu)
                    //&& mirSchedule->requestList.at(i)->state < 2)    /*找到了当前病区（不是执行完的）*/
            {
                if(((i+1) <= (mirSchedule->requestList.size() -1)) )    /*判断后面还有请求？*/
                {
                    for(int j = i+1; j < mirSchedule->requestList.size(); j++)  /*遍历后边的请求*/
                    {
                        if(mirSchedule->requestList.at(j)->state == 0)   /*保证未开始*/
                        {
                            /*仅仅紧接着的请求go=1，这之后的不管*/
                            mirSchedule->requestList.at(j)->goCmd = 1;
                            return true;    /*true表明，有请求go=1了*/
                        }/*在遍历的过程中，确定未执行的bingqu*/
                    }/*遍历后面的病区*/
                }/*锁定病区发起者的前提下，确定后面还有bingqu*/
            }/*锁定了当前病区发起者*/
        }
    }
    return false;   /*false表明，此次操作没有生效*/
}
QString  RequestQueueCON::GetExecutingRequest()
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->state == 1)
            return item->bingqu;
    }
    return QStringLiteral("");
}
QString  RequestQueueCON::GetFirstNotExecuteRequest()
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->state < 2)
            return item->bingqu;
    }
    return "";
}
bool RequestQueueCON::HasNotexecDest(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->state == 0)
                    return true;
            }
        }
    }
    return false;
}
bool RequestQueueCON::HasExecDest(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->state == 1)
                    return true;
            }
        }
    }
    return false;
}
bool RequestQueueCON::HasNotexecRequest()
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->state == 0)
            return true;
    }
    return false;
}
bool RequestQueueCON::HasNotexecNormalRequest()
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->state < 3 && item->type < 3)
            return true;
    }
    return false;
}
/*请求中是否有未执行完的目的地？*/
QString RequestQueueCON::GetNotexecDest(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->state == 0)
                    return node->bingqu;
            }
        }
    }
    return QStringLiteral("");
}
/*请求中是否都已经完场*/
bool RequestQueueCON::AllDestComplete(QString bingqu)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->state < 2)
                    return false;
            }
        }
    }
    return true;
}
//有待改进 只是state的不同
QString RequestQueueCON::GetExecutingDest(QString bingqu)
{
    //遍历requestList 如果state == 1
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state == 1)
        {
            foreach (DestNode * node, item->destList) {
                if(node->state == 1)
                    return node->bingqu;
            }
        }
    }
    return QStringLiteral("");
}

bool RequestQueueCON::CalculateDistance(QString bingqu, QString dest, double x, double y)
{
    double value = 0;

    //遍历requestList
    qDebug()<<bingqu<<dest<<" " + QString::number(x)<<" " + QString::number(y);
    foreach (RequestNode * item, mirSchedule->requestList) {
        qDebug()<<"1";
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    //首先更新此dest的坐标(有可能插入的时候不正确，双重确认)
                    node->posX = mirSchedule->GetPositonX(node->bingqu);
                    node->posY = mirSchedule->GetPositonY(node->bingqu);

                    qDebug()<<"4";
                    qDebug()<<" " +QString::number(node->posX)<<" " + QString::number(node->posY);
                    value = (node->posX - x) * (node->posX - x);
                    value += (node->posY - y) * (node->posY - y);
                    node->distance = sqrt(value);
                    return true;
                }
            }
        }
    }
    return false;
}
double RequestQueueCON::GetDistance(QString bingqu, QString dest)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    return node->distance;
                }
            }
        }
    }
    return 10000.0;
}

bool RequestQueueCON::SetDestState(QString bingqu, QString dest, int state)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    node->state = state;
                    return true;
                }
            }
        }
    }
    return false;
}


RequestNode * RequestQueueCON::CreateRequestNode(MIRSchedule * parent,
                                QString bingqu,
                                QString requestContent)
{
    RequestNode * rNode = nullptr;
    DestNode * dNode = nullptr;

    //由bingqu 确认所有参数
    /*1	任意手术室
    * 2	材料室
    * 3	集散地
    */

    if(bingqu == QStringLiteral("集散地"))
    {
        rNode = RequestNodeFactory::instance(parent);
        dNode = DestNodeFactory::instance(parent);
        //设置dNode
        dNode->bingqu = bingqu;
        dNode->state = 0;   //0未执行 1执行中 2执行完
        dNode->posX = parent->GetPositonX(bingqu);
        dNode->posY = parent->GetPositonY(bingqu);
        dNode->distance = 0xffff;
        //设置rNode
        rNode->type = 3;
        rNode->bingqu = bingqu;
        rNode->goCmd = 0;
        rNode->state = 0;       //0未执行 1执行中 2执行完
        rNode->requestContent.clear();
        rNode->destList.append(dNode);
        return rNode;
    }
    if(bingqu == QStringLiteral("充电站"))
    {
        rNode = RequestNodeFactory::instance(parent);
        dNode = DestNodeFactory::instance(parent);
        //设置dNode
        dNode->bingqu = "充电站";
        dNode->state = 0;   //0未执行 1执行中 2执行完
        dNode->posX = parent->GetPositonX(bingqu);
        dNode->posY = parent->GetPositonY(bingqu);
        dNode->distance = 0xffff;
        //设置rNode
        rNode->type = 4;            //充电站作为普通病区处理
        rNode->bingqu = bingqu;
        rNode->goCmd = 0;
        rNode->state = 0;       //0未执行 1执行中 2执行完
        rNode->requestContent.clear();
        rNode->destList.append(dNode);
        return rNode;
    }
    else if(bingqu == QStringLiteral("材料室"))
    {
        rNode = RequestNodeFactory::instance(parent);
        dNode = DestNodeFactory::instance(parent);
        //设置dNode
        dNode->bingqu = bingqu;
        dNode->state = 0;   //0未执行 1执行中 2执行完
        dNode->posX = parent->GetPositonX(bingqu);
        dNode->posY = parent->GetPositonY(bingqu);
        dNode->distance = 0xffff;
        //设置rNode
        rNode->type = 2;
        rNode->bingqu = bingqu;
        rNode->goCmd = 0;
        rNode->state = 0;       //0未执行 1执行中 2执行完
        rNode->requestContent.clear();
        rNode->destList.append(dNode);
        return rNode;
    }
    else
    {
        rNode = RequestNodeFactory::instance(parent);
        dNode = DestNodeFactory::instance(parent);
        //设置dNode1
        dNode->bingqu = bingqu;
        dNode->state = 0;   //0未执行 1执行中 2执行完
        dNode->posX = parent->GetPositonX(bingqu);
        dNode->posY = parent->GetPositonY(bingqu);
        dNode->distance = 0xffff;
        //设置rNode
        rNode->type = 1;
        rNode->bingqu = bingqu;
        rNode->goCmd = 0;
        rNode->state = 0;       //0未执行 1执行中 2执行完
        rNode->requestContent = requestContent;
        rNode->destList.append(dNode);
        //设置dNode2
        dNode = DestNodeFactory::instance(parent);
        dNode->bingqu = QStringLiteral("材料室");
        dNode->state = 0;   //0未执行 1执行中 2执行完
        dNode->posX = parent->GetPositonX(QStringLiteral("材料室"));
        dNode->posY = parent->GetPositonY(QStringLiteral("材料室"));
        dNode->distance = 0xffff;
        rNode->destList.append(dNode);

        return rNode;
    }

}
bool RequestQueueCON::SetRequestState(QString bingqu, int state)
{
    //遍历requestList state 0未执行 1执行中 2执行完
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            item->state = state;
            return true;
        }
    }
    return false;
}
int RequestQueueCON::GetRequestState(QString bingqu)
{
    //遍历requestList state 0未执行 1执行中 2执行完
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
            return item->goCmd;
    }
    return 0;
}
bool RequestQueueCON::PlusDestTick(QString bingqu, QString dest)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    node->timeTick++;
                    return true;
                }
            }
        }
    }
    return false;
}
int RequestQueueCON::GetDestTick(QString bingqu, QString dest)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    return node->timeTick;
                }
            }
        }
    }
    return 0;
}
bool RequestQueueCON::ResetDestTick(QString bingqu, QString dest)
{
    //遍历requestList
    foreach (RequestNode * item, mirSchedule->requestList) {
        if(item->bingqu == bingqu && item->state < 2)
        {
            foreach (DestNode * node, item->destList) {
                if(node->bingqu == dest)
                {
                    node->timeTick = 0;
                    return true;
                }
            }
        }
    }
    return false;
}
