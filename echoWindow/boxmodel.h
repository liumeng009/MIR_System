#ifndef BOXMODEL_H
#define BOXMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "chartform.h"
#include "applyform/applyform.h"
#include "boxunit.h"
#include "rms.h"

class BoxModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit BoxModel(QObject *parent = 0);

    //单例依赖
    RMS * rms = nullptr;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
};


#endif // BoxModel_H
