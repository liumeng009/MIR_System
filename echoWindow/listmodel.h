#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include "applyform/applyform.h"

class ListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ListModel(QObject *parent = 0,
                       ApplyUnit * inApplyUnit = nullptr);

    //单例依赖
    ApplyUnit * applyUnit = nullptr;

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
};

#endif // LISTMODEL_H
