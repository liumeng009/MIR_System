#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QModelIndex>
#include "applyform/applyform.h"
#include "itemdef.h"

class ChartModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChartModel(QObject *parent = 0, ApplyUnit * inApplyUnit = 0);

    //单例依赖
    ApplyUnit * applyUnit = nullptr;

    //成员
    int indexNum = 0;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void OneCellChanged();
private:
public slots:
    void IndexChange(QModelIndex index);
};


#endif // CHARTMODEL_H
