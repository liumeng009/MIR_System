#ifndef SQLDEMO_H
#define SQLDEMO_H

#include <QtGui>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include "TableModel.h"
#include "TableView.h"
#include "delegate.h"

/*SqlModule针对的是数据库中的表格，一个表格一个SqlModule*/
class SqlModule : public QObject
{
    Q_OBJECT

public:
    QSqlDatabase dataBase;

    QList<QSqlRecord> recordList;
    QTimer * demoTimer;

    //定义所有的model，一个表格一个model，
    TableModel * model;

    //定义所有的view，一个表格一个view
    TableView * view;

    //定义所有代理，一个表格一个代理
    ControlSchemeDelegate * controlSchemeDelegate;

    SqlModule(QString tableName);   /*初始化就要提供表格名称*/
    ~SqlModule();

    TableModel * GetDataModel();
    TableView * GetDataView();


public slots:
    /*按键对应的操作*/
    void addButtonFunc();
    void delButtonFunc();
    void selectButtonFunc();
    void submitButtonFunc();

    void CreateTable();
    void InsertData();
    void UpdateData();
    void UpdateData(QString id, QString fingerInfo);
    void SelectData();

    void RecordUpdate();
signals:
    void DataChanged();
public:
    QList<QSqlRecord> * GetRecordList();
};

#endif // SQLDEMO_H
