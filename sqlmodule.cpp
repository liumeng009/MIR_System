#include "sqlmodule.h"

SqlModule::SqlModule(QString tableName)
{
    /*首先连接数据库，如果第一次连接就创建,如果第二次就使用就连接*/
    /*model的结构是由sql中的表来决定的，不用定义（写一个设置表的接口）*/
    /*写一个返回widget的接口，便于displayDebug组织*/

    if(QSqlDatabase::contains("comm"))
    {
        dataBase = QSqlDatabase::database("comm");
        qDebug()<<"open old comm";
        /*banging model*/
        model = new TableModel(nullptr ,dataBase);

        model->setTable(tableName);  //指定模型要关联的表
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();  //查询表格数据
    }
    else
    {
        dataBase = QSqlDatabase::addDatabase("QSQLITE","comm");
        dataBase.setDatabaseName("my.db");
        dataBase.setUserName("lemon");
        //dataBase.setPassword("123456");
        if(dataBase.open())
        {
            qDebug()<<"OPEN";
            /*banging model*/
            model = new TableModel(this ,dataBase);

            model->setTable(tableName);  //指定模型要关联的表
            model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        }
        else
        {
            qDebug()<<"error";
            model = NULL;
        }
    }
    if(model->tableName() == "病区表")
    {
        model->setHeaderData(0, Qt::Horizontal, tr("病区id"));
        model->setHeaderData(1, Qt::Horizontal, tr("病区名称"));

        //view
        view = new TableView;

        //delegate
        controlSchemeDelegate = new ControlSchemeDelegate;

        //绑定所有model和view
        view->setModel(model);


        //绑定所有的view和delegate
        view->setItemDelegate(controlSchemeDelegate);

    }
    else if(model->tableName() == "医护表")
    {
        model->setHeaderData(0, Qt::Horizontal, tr("员工id"));
        model->setHeaderData(1, Qt::Horizontal, tr("员工姓名"));
        model->setHeaderData(2, Qt::Horizontal, tr("所属病区id"));

        //view
        view = new TableView;

        //delegate
        controlSchemeDelegate = new ControlSchemeDelegate;

        //绑定所有model和view
        model->removeColumn(3); /*特殊处理*/
        view->setModel(model);


        //绑定所有的view和delegate
        view->setItemDelegate(controlSchemeDelegate);

    }
    else if(model->tableName() == "指纹表")
    {
        model->setHeaderData(0, Qt::Horizontal, tr("员工id"));
        model->setHeaderData(1, Qt::Horizontal, tr("指纹信息"));

        //view
        view = new TableView;

        //delegate
        controlSchemeDelegate = new ControlSchemeDelegate;

        //绑定所有model和view
        view->setModel(model);


        //绑定所有的view和delegate
        view->setItemDelegate(controlSchemeDelegate);

    }
    else if(model->tableName() == "箱体配置表")
    {
        model->setHeaderData(0, Qt::Horizontal, tr("箱体id"));
        model->setHeaderData(1, Qt::Horizontal, tr("MIR的ip"));
        model->setHeaderData(2, Qt::Horizontal, tr("MIR的port"));

        //view
        view = new TableView;

        //delegate
        controlSchemeDelegate = new ControlSchemeDelegate;

        //绑定所有model和view
        model->removeColumn(3); /*特殊处理*/
        view->setModel(model);


        //绑定所有的view和delegate
        view->setItemDelegate(controlSchemeDelegate);
    }
    model->select();  //查询表格数据

    demoTimer = new QTimer(this);
    QObject::connect(demoTimer,SIGNAL(timeout()),this,SLOT(RecordUpdate()));
    demoTimer->start(2000);

    //connect(controlSchemeDelegate,SIGNAL(ModelDataChanged()),
           // model,SLOT(submitAll()));

}
SqlModule::~SqlModule()
{
    dataBase.close();
    qDebug()<<"database close";
}

void SqlModule::addButtonFunc()
{
    //start transaction
    //model->database().transaction();
    QSqlRecord record = model->record();
    model->insertRecord(-1, record);
}
void SqlModule::delButtonFunc()
{
    // 通过_view去获取被选中的部分的数据model
    QItemSelectionModel * selectModel = view->selectionModel();
    // 通过选中的数据结构，获取这些格子的ModelIndex
    QModelIndexList selectList =  selectModel->selectedIndexes();

    QList<int> delRow;
    // 遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    for(int i=0; i<selectList.size(); ++i)
    {
        QModelIndex index = selectList.at(i);
        //  _model->removeRow(index.row());
        delRow << index.row();  //将行加到  list 里
    }//排除重复行
    while(delRow.size() > 0)
    {
        int row = delRow.at(0);
        delRow.removeAll(row);
        model->removeRow(row);
    }

    //删除model后，先提交更改，然后再刷新一遍
    model->submitAll();
    model->select();
}
void SqlModule::selectButtonFunc()
{
    model->select();
}
void SqlModule::submitButtonFunc()
{
    if(model->submitAll())
    {
        /*提交更改后，要发出信号通知大家*/
        emit DataChanged();
    }
    else
    {
        /*失败则不做，可以在界面提醒*/
    }

    model->select();
}

void SqlModule::CreateTable()
{
    QSqlQuery sqlQuery(dataBase);   //指定数据库
    QString create_sql = "create table student( id int primary key, name varchar(30), age int)";
    sqlQuery.prepare(create_sql);
    if(sqlQuery.exec())
    {
        qDebug()<<"create !";
    }
    else
    {
        qDebug()<<"create failed";
    }
}
void SqlModule::InsertData()
{
    QSqlQuery sqlQuery(dataBase);   //指定数据库
    QString create_sql = "insert into student values (?,?,?)";
    sqlQuery.prepare(create_sql);

    sqlQuery.addBindValue(1);
    sqlQuery.addBindValue("wang");
    sqlQuery.addBindValue(25);

    if(sqlQuery.exec())
    {
        qDebug()<<"insert !";
    }
    else
    {
        qDebug()<<"insert failed";
    }
}
void SqlModule::UpdateData()
{
    QSqlQuery sqlQuery(dataBase);   //指定数据库
    QString create_sql = "update student set name = :name where id = :id";
    sqlQuery.prepare(create_sql);

    sqlQuery.bindValue(":name","Qt");
    sqlQuery.bindValue(":id","1");

    if(sqlQuery.exec())
    {
        qDebug()<<"update !";
    }
    else
    {
        qDebug()<<"update failed";
    }
}
/*更新指纹信息的操作*/
void SqlModule::UpdateData(QString id, QString fingerInfo)
{
    QSqlQuery sqlQuery(dataBase);   //指定数据库
    QString create_sql = "update 医护表 set finger = :finger where id = :id";
    sqlQuery.prepare(create_sql);

    sqlQuery.bindValue(":finger",fingerInfo);
    sqlQuery.bindValue(":id", id);

    if(sqlQuery.exec())
    {
        qDebug()<<"update !";
    }
    else
    {
        qDebug()<<"update failed";
    }
}
void SqlModule::SelectData()
{
    QSqlQuery sqlQuery(dataBase);   //指定数据库
    QString create_sql = "select id,name from student";
    sqlQuery.prepare(create_sql);

    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            qDebug()<<sqlQuery.value(0).toString()<<sqlQuery.value(1).toString();
        }
    }
    else
    {
        qDebug()<<"select failed";
    }
}
TableModel * SqlModule::GetDataModel()
{
    return model;
}
TableView * SqlModule::GetDataView()
{
    return view;
}
QList<QSqlRecord> * SqlModule::GetRecordList()
{
    return &recordList;
}
void SqlModule::RecordUpdate()
{
    //if(model->select())
    //{
    if(model->rowCount() != 0)
    {
        recordList.clear(); //select成功后，先清空

        for (int i = 0; i < model->rowCount(); i++)
        {
            QSqlRecord record = model->record(i);
            recordList.append(record);
        }
    }
    //}

}
