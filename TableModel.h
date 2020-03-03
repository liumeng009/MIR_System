#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlTableModel>
#include <QObject>
#include <QSqlDatabase>
#include <QDebug>

class TableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    //explicit TableModel(QObject *parent = 0);
    TableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    //int rowCount(const QModelIndex &parent) const;
    //int columnCount(const QModelIndex &parent) const;
    //QVariant data(const QModelIndex &index, int role) const;
    //Qt::ItemFlags flags(const QModelIndex &index) const;
    //void setHorizontalHeader(const QStringList& headers);
    //QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //void setData(const QVector<QStringList>& data);
    //QVector<QStringList>& DataVector() {return m_data;}
    ~TableModel(void);

signals:

public slots:

private:
    QStringList m_HorizontalHeader;
    QVector<QStringList> m_data;
    //关于所有表的定义（数据结构）
};

#endif // TABLEMODEL_H
