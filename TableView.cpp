#include "tableview.h"
#include <QHeaderView>
#include "tablemodel.h"

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
    /*每一列设置自适应宽度*/
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

TableView::~TableView()
{
//    delete m_model;
}
/*
void TableView::iniData()
{
    m_model = new TableModel();
    this->setModel(m_model);    //设置view的model
    QStringList headers;    //定义一个Qstring列表
    headers << "Id" << "Progress"<<"Button";
    m_model->setHorizontalHeader(headers);
    //model设置列表！不是view 系统功能

    QVector<QStringList> data;
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    data.append(QStringList() << "3" << "2"<<"1");
    data.append(QStringList() << "4" << "80"<<"1");
    data.append(QStringList() << "5" << "40"<<"1");
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    data.append(QStringList() << "3" << "2"<<"1");
    data.append(QStringList() << "4" << "80"<<"1");
    data.append(QStringList() << "5" << "40"<<"1");
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    data.append(QStringList() << "3" << "2"<<"1");
    data.append(QStringList() << "4" << "80"<<"1");
    data.append(QStringList() << "5" << "40"<<"1");
    data.append(QStringList() << "1" << "22"<<"1");
    data.append(QStringList() << "2" << "32"<<"1");
    m_model->setData(data);
    //设置model中的内容！以后要用好的设计结构

    //定义代理（button功能）也可以定义更多代理
    m_buttonDelegate = new ButtonDelegate(this);
    //定义一个代理
    this->setItemDelegateForColumn(2, m_buttonDelegate);
    //给view的第一行设置代理！
    emit m_model->layoutChanged();//发送一个信号
    this->setColumnWidth(1, 500);
    //设置VIEW的宽度 不重要
}
*/
