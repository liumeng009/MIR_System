#ifndef ITEMDEF_H
#define ITEMDEF_H
#include <QMetaType>

typedef struct
{
    QString categoryName;
    QString specsName;
    int increaseMount;
    int stockMount;
}ItemData;


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

Q_DECLARE_METATYPE(ItemData)
Q_DECLARE_METATYPE(DrawerData)
Q_DECLARE_METATYPE(CellData)
#endif // ITEMDEF_H
