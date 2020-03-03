#ifndef ITEMDEF_H
#define ITEMDEF_H

#include <QMetaType>
#include <QString>

#define SM_INSERT_REQUEST       1
#define SM_DELETE_REQUEST       2
#define SM_PROCESS_REQUEST      3
#define SM_UPDATE_REQUEST       4
#define SM_GET_ALL_POSITION     5
#define SM_GET_MISSION          6
#define SM_GO_CMD_REQUEST       7
#define SM_OPEN_CMD_REQUEST     8
#define SM_TIME_OUT             9
#define SM_RETRY_REQUEST        10
#define SM_CHARGE_REQUEST       11
#define SM_CLEAR_ERROR_REQUEST  12

#define BOX_REQUEST_INSERT          1
#define BOX_REQUEST_DELETE          2
#define BOX_REQUEST_STATUS_QUERY    3
#define BOX_REQUEST_STOCK_QUERY     4
#define BOX_REQUEST_TIMEOUT         5


// type_id定义
#define JSON_TYPE_SUBMIT 1   //提交订单命令 平板提交订单
#define JSON_TYPE_CANCEL 2   //取消订单命令 平板取消已叫转运箱
#define JSON_TYPE_STATUS_QUERY  3   //请求当前状态 平板请求获得转运箱运行状态和请求状态
#define JSON_TYPE_STOCK_QUERY   4   //获得库存
#define JSON_TYPE_TIMEOUT   5   //超时

typedef struct
{
    QString categoryName;
    QString specsName;
    int increaseMount;
    int stockMount;
}ItemData;                  //购物车项目

typedef struct
{
    int drawerNum;
    int checked;            //是否被选中
    QString categoryName;   //内容
}DrawerData;

typedef struct
{
    int checked;            //是否被选中
    QString specsName;      //内容
    int increaseMount;      //数量
    int stockMount;         //库存
    int referStockAmount;   //参考库存数量
    int supplyAmount;
    int estimateStockAmount;
}CellData;

typedef struct
{
    QString MIRTargetBingqu;
    int MIRState;
    QString stateId;       //http刷新 当前状态编号
    QString stateText;     //http刷新 当前状态文本
    QString missionText;    //http刷新 当前任务文本状态*/
    QString missionState;
    bool requestExist;      //请求是否存在，针对bingqu计算生成
}StateData;

typedef struct
{
    int boxNum;
    double battery_percentage;
}BoxData;

typedef struct  /*全部传送*/
{
    uint16_t id;
    uint8_t name[8];
    uint16_t bingquId;
    QString strName;
    QString strFinger;
}YihuData;

Q_DECLARE_METATYPE(ItemData)
Q_DECLARE_METATYPE(DrawerData)
Q_DECLARE_METATYPE(CellData)
Q_DECLARE_METATYPE(StateData)
Q_DECLARE_METATYPE(BoxData)
Q_DECLARE_METATYPE(YihuData)


#endif // ITEMDEF_H
