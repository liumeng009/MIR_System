#include "listmodel.h"

ListModel::ListModel(QObject *parent, ApplyUnit * inApplyUnit)
    : QAbstractListModel(parent)
    ,applyUnit(inApplyUnit)
{

}
int ListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
int ListModel::rowCount(const QModelIndex &parent) const
{
    return applyUnit->DrawerUnitList.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        qDebug()<<"-----"<<index.row();
        if( 0 <= index.row() && 0 < applyUnit->DrawerUnitList.size())
            return applyUnit->GetDrawerName(index.row());
        else
            return QVariant();
    }
    else
        return QVariant();
}

