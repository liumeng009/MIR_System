#include "boxmodel.h"
#include "itemdef.h"
#include "rmsfactory.h"


BoxModel::BoxModel(QObject *parent)
  : QAbstractListModel(parent)
  ,rms(RMSFactory())
{

}
int BoxModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
int BoxModel::rowCount(const QModelIndex &parent) const
{
    return rms->boxMap.size();
}

QVariant BoxModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if( 0 <= index.row() && rms->boxMap.size() > 0)
        {
            BoxData item;

            //准备索引
            QMap<int,BoxUnit *>::iterator it;
            it = rms->boxMap.begin();

            //定位+取数据
            it += index.row();
            item.boxNum = it.key();
            item.battery_percentage = it.value()->mirSchedule->state.battery_percentage;

            return QVariant::fromValue(item);
        }
        else
            return QVariant();
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
bool BoxModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::DisplayRole)
        return false;

//    if(role == Qt::EditRole && value.toInt() == 1)
//    {
//        emit layoutChanged();
//        return true;
//    }
//    if(role == Qt::EditRole && value.toInt() == 2)
//    {
//        emit layoutChanged();
//        return true;
//    }
    return true;
}

Qt::ItemFlags BoxModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsSelectable|QAbstractItemModel::flags(index);
}
