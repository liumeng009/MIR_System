#ifndef REQUESTNODE_H
#define REQUESTNODE_H

#include <QObject>

class DestNode;

class RequestNode : public QObject
{
    Q_OBJECT
public:
    explicit RequestNode(QObject *parent = nullptr);

    //依赖

    //成员
    int     type = 0;   //1.正常区域 2.材料 3.集散 4.充电
    QString bingqu;     //发起者病区 比如材料室，普通病区，其实和type功能有重叠，但更具体
    int     goCmd = 0;  //可发车命令
    int     state = 0;  //0未执行 1执行中 2执行完
    int     opened = 0; //是否已被打开
    QString requestContent; //请求的内容，以JSON形式缓存

    QList <DestNode *> destList;    //发起者所要到达的实际目的地 dest1 dest2 ....
signals:

public slots:
};
/*普通工厂*/
class RequestNodeFactory
{
public:
    static RequestNode * instance(QObject *parent = nullptr)
    {
        return new RequestNode(parent);;
    }
};

class DestNode : public QObject
{
    Q_OBJECT
public:
    explicit DestNode(QObject *parent = nullptr);

    //成员
    //int     bingquNum;        //病区编号，实际并没有什么用
    QString bingqu;         //实际的节点地址，比如 材料室、集散地，（真实下发时采用的地址！）
    int     state;          //0未执行 1执行中 2执行完
    double     posX;        //此地点在map上的X坐标。
    double     posY;        //此地点在map上的Y坐标。
    double  distance;       //此地点和MIR机器人的距离（不断由X，Y坐标进行计算）。
    int     timeTick = 0;   //在移动到此地点的过程中耗费的时间（为了超时控制）
signals:

public slots:

};
/*普通工厂*/
class DestNodeFactory
{
public:
    static DestNode * instance(QObject *parent = nullptr)
    {
        return new DestNode(parent);;
    }
};

#endif // REQUESTNODE_H
