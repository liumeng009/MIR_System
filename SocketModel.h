#ifndef SOCKETMODEL_H
#define SOCKETMODEL_H

#include <QAbstractTableModel>
#include "table.h"


class SocketModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SocketModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setHorizontalHeader(const QStringList& headers);//设置标题
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setData(QList <TcpItem> * data); //设置数据源
    //QVector<QStringList>& DataVector() {return m_data;}
    ~SocketModel(void);

signals:

public slots:

private:
    QStringList m_HorizontalHeader;

    /*tcpItemList是由server维护的，这里只是调用*/
    QList <TcpItem> * tcpItemList;
};

#endif // TABLEMODEL_H
