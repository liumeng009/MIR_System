#include "SocketModel.h"


SocketModel::SocketModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

SocketModel::~SocketModel()
{

}


int SocketModel::rowCount(const QModelIndex &parent) const
{
    return tcpItemList->size();
}

int SocketModel::columnCount(const QModelIndex &parent) const
{
    //return m_HorizontalHeader.count();
    return 4;
}

//data是被不停调用的 不停访问m_data
QVariant SocketModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole)
    {
        int ncol = index.column();
        int nrow =  index.row();
        //TcpItem values = tcpItemList->at(nrow);
        //qDebug()<<"nrow"<<nrow;
        switch (ncol) {

        default:
            break;
        }
        /*if (values.size() > ncol)
            return values.at(ncol);
        else
            return QVariant();*/
    }
    return QVariant();
}

Qt::ItemFlags SocketModel::flags(const QModelIndex &index) const
{
    /*if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    // flag|=Qt::ItemIsEditable // 设置单元格可编辑,此处注释,单元格无法被编辑
    return flag;*/
}

void SocketModel::setHorizontalHeader(const QStringList &headers)
{
    //m_HorizontalHeader =  headers;
}


QVariant SocketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    /*if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_HorizontalHeader.at(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
    */
}

void SocketModel::setData(QList <TcpItem> * data)
{
    tcpItemList = data;//只是把数据源赋给了成员变量
}
