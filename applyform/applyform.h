#ifndef APPLYFORM_H
#define APPLYFORM_H

#include <QObject>
#include <QPoint>
#include <QList>
#include <QDebug>
#include <QVariant>
#include "itemdef.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class DrawerUnit;
class CellUnit;

/*ApplyUnit作为主要数据结构，是操作的主体*/
class ApplyUnit : public QObject
{
    Q_OBJECT
public:
    explicit ApplyUnit(QObject *parent = nullptr);
    ~ApplyUnit();

    int boxNum;
    QString bingqu;
    QString MIRIP;
    int MIRPORT;
    uint8_t enabled;
    uint8_t checked;
    QList<DrawerUnit *> DrawerUnitList;
signals:
    void OneCellChanged();
public slots:
    void ClearData();
    void PrintData();
    void FromJson(QJsonObject json);
    QJsonObject ToJson();

    /*获取库存*/
    int GetStockAmount(int boxNum, int drawerNum, int cellNum);
    int GetIncreaseAmount(int boxNum, int drawerNum, int cellNum);
    /*减去一个applyUnit*/
    bool Minus(ApplyUnit * applyUnitS);
    bool LessThan(ApplyUnit * applyUnitS);

    /*添加到增量 按内容而非索引*/
    bool AddOne(int boxNum, int drawerNum, int cellNum);
    bool RemoveOne(int boxNum, int drawerNum, int cellNum);
    bool SetOne(int indexBoxNum,
                int indexDrawerNum,
                int indexCellNum,
                int increaseMount);
    bool RemoveAll();
    bool EStockToStock();

    /*更新到存量 按内容而非索引*/
    bool AddN(int boxNum, int drawerNum, int cellNum, int num);
    bool RemoveN(int boxNum, int drawerNum, int cellNum, int num);

    /*统计函数*/
    int GetTotalCellNum();
    int GetTotalCheckedCellNum();

    /*获取内容函数*/
    DrawerData GetDrawerData(int drawerNum);
    CellData GetCellData(int drawerNum, int cellNum);
    //没有model view的话 更新数据确实比较麻烦

    bool Rulecheck();

    QVariant GetDrawerName(int drawerNum);
    QVector<ItemData> GetChechedCells();
};

class ApplyUnitFactory
{
public:
    static ApplyUnit * instance(QObject * parent = nullptr)
    {
        return new ApplyUnit(parent);
    }
};

class DrawerUnit : public QObject
{
    Q_OBJECT
    //Q_DISABLE_COPY(DrawerUnit)
public:
    explicit DrawerUnit(QObject *parent = nullptr);
    ~DrawerUnit();

    uint16_t DrawerNum;
    QString layoutFile;
    QString categoryName;
    uint8_t enabled;
    uint8_t checked;
    QList<CellUnit *> CellUnitList;
signals:

public slots:
};
/*暂时不要使用工厂，有问题，待改正*/
class DrawerUnitFactory
{
public:
    static DrawerUnit * instance(QObject *parent = nullptr)
    {
        return new DrawerUnit(parent);
    }
};

class CellUnit : public QObject
{
    Q_OBJECT
    //Q_DISABLE_COPY(CellUnit)
public:
    explicit CellUnit(QObject *parent = nullptr);
    ~CellUnit();

    uint16_t CellNum;
    int pos_leftup_X;
    int pos_leftup_Y;
    int widget_width;
    int widget_height;
    QString specsName;
    int stockAmount;
    int increaseAmount;
    int estimateStockAmount;
    int referStockAmount;
    int supplyAmount;
    int lackAmount;
    int lackState;
    uint8_t enabled;
    uint8_t checked;
signals:

public slots:
};
class CellUnitFactory
{
public:
    static CellUnit * instance(QObject *parent = nullptr)
    {
        return new CellUnit(parent);
    }
};

#endif // APPLYFORM_H
