#include "chartmodel.h"


ChartModel::ChartModel(QObject *parent , ApplyUnit * inApplyUnit)
  : QAbstractListModel(parent)
    ,applyUnit(inApplyUnit)
{
    connect(this,SIGNAL(OneCellChanged()),
            applyUnit,SIGNAL(OneCellChanged()));
}
int ChartModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
int ChartModel::rowCount(const QModelIndex &parent) const
{
    //判断DrawerUnitList是否为空，判断是否drawerNum有效
    if(applyUnit->DrawerUnitList.size() == 0)
        return 0;
    else
    {
        if(indexNum < applyUnit->DrawerUnitList.size())
        {
            //获得drawerNum的大小
            return applyUnit->DrawerUnitList.at(indexNum)->CellUnitList.size();
        }
        else
            return 0;
    }
}

QVariant ChartModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //获得有效的drawer索引
        if(applyUnit->DrawerUnitList.size() == 0)
            return QVariant();
        else
        {
            if(indexNum < applyUnit->DrawerUnitList.size())
            {
                ItemData item;

                item.specsName = applyUnit->DrawerUnitList.at(indexNum)->CellUnitList.at(index.row())->specsName;
                item.increaseMount = applyUnit->DrawerUnitList.at(indexNum)->CellUnitList.at(index.row())->increaseAmount;
                item.stockMount = applyUnit->DrawerUnitList.at(indexNum)->CellUnitList.at(index.row())->stockAmount;

                return QVariant::fromValue(item);
            }
            else
                return QVariant();
        }
    }
    else if (role == Qt::EditRole)
    {
//        qDebug()<<"add one";
//        chartForm->AddOne(chartForm->chartList.at(index.row())->boxNum,
//                          chartForm->chartList.at(index.row())->drawerNum,
//                          chartForm->chartList.at(index.row())->cellNum);
    }
    else
        return QVariant();
}
bool ChartModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::DisplayRole)
        return false;

    if(role == Qt::EditRole && value.toInt() == 1)
    {
        applyUnit->DrawerUnitList[indexNum]->CellUnitList[index.row()]->increaseAmount++;
        applyUnit->Rulecheck();
        emit OneCellChanged();
        emit layoutChanged();   //通知自己的视图更新
        return true;
    }
    if(role == Qt::EditRole && value.toInt() == 2)
    {
        applyUnit->DrawerUnitList[indexNum]->CellUnitList[index.row()]->increaseAmount--;
        applyUnit->Rulecheck();
        emit OneCellChanged();
        emit layoutChanged();   //通知自己的视图更新
        return true;
    }
}

Qt::ItemFlags ChartModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsSelectable|QAbstractItemModel::flags(index);
}

void ChartModel::IndexChange(QModelIndex index)
{
    qDebug()<<index.row();

    /*仓格改变*/
    indexNum = index.row();
    emit layoutChanged();
}
