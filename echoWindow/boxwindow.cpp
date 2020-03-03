#include "boxwindow.h"
#include "ui_boxwindow.h"
#include <QVariant>
#include <QMenu>
#include <QAction>
#include "boxmodel.h"
#include "boxitemdelegte.h"
#include "echoWindow/stockwindow.h"
#include "echo/rmswindow.h"
#include "controller/boxcon.h"
#include "itemdef.h"
#include "rmsfactory.h"
#include "faderwidget.h"

BoxWindow::BoxWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoxWindow)
  ,timer(new QTimer(this))
{
    ui->setupUi(this);

    pModel = new BoxModel(this);
    BoxItemDelegte *pItemDelegate = new BoxItemDelegte(this);

    ui->listView->setUpdatesEnabled(false);
    ui->listView->setModel(pModel);
    ui->listView->setItemDelegate(pItemDelegate);
    ui->listView->setResizeMode(QListView::Adjust);
    //ui->listView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->listView->setUpdatesEnabled(true);

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(show_contextmenu(const QPoint&)));

    /*定时刷新界面*/
    connect(timer,&QTimer::timeout,
            this,[this](){
        emit pModel->layoutChanged();   /*model定时发出刷新信号，促使view更新*/
    });
    timer->start(5000);

    /*button点击，执行相应创建库存window和呼叫任务*/
    connect(pItemDelegate,SIGNAL(ButtonClicked(int)),
            this,SLOT(CreateStockWindow(int)));

    connect(pItemDelegate,SIGNAL(RequstButtonClicked(int)),
            this,SLOT(RequestCall(int)));
}
void BoxWindow::CreateConnectFunc()
{
    /*配置界面呼出*/
    connect(ui->pushButton1,&QPushButton::clicked,
            this,[](){
        BingquWindowFactory::instance()->show();
    });
    connect(ui->pushButton2,&QPushButton::clicked,
            this,[](){
        YihuWindowFactory::instance()->show();
    });
    connect(ui->pushButton3,&QPushButton::clicked,
            this,[](){
        XiangtiWindowFactory::instance()->show();
    });
    connect(ui->pushButton4,&QPushButton::clicked,
            this,[](){
        FingerToolWindowFactory::instance()->show();
    });
    connect(ui->pushButton5,&QPushButton::clicked,
            this,[](){
        SysConfigWindowFactory::instance()->show();
    });
    connect(ui->pushButton6,&QPushButton::clicked,
            this,[](){
        ReportWindowFactory::instance()->show();
    });
}

void BoxWindow::keyPressEvent(QKeyEvent *event)
{
    // 捕捉 Ctrl + Alt + A 组合键;
    if (event->key() == Qt::Key_D  &&  event->modifiers() == (Qt::AltModifier | Qt::ControlModifier))
    {
        if(MirDebugFactory::instance() != nullptr)
            MirDebugFactory::instance()->show();
    }
    if (event->key() == Qt::Key_C  &&  event->modifiers() == (Qt::AltModifier | Qt::ControlModifier))
    {
        if(MirDebugFactory::instance() != nullptr)
            MirDebugFactory::instance()->hide();
    }
}
void BoxWindow::RequestCall(int buttonNum)
{
    //在此利用了CmdStateMachine，直接操作
    //本来是统一通过boxCON进行操作，但是由于“材料室"的多重身份
    //因此要根据buttonNUm区别对待，得出各个的schedule系统
    //所以要用CmdStateMachine类。
    if(RMSFactory()->GetBoxUnit(buttonNum))
    {
        CmdStateMachine * sm = CmdStateMachineFactory::instance(this,RMSFactory()->GetBoxUnit(buttonNum)->mirSchedule);
        connect(sm,&CmdStateMachine::CmdSMFinished,
                this,std::bind(&BoxWindow::CmdSMFinished,this,sm,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        sm->InsertRequest(QStringLiteral("材料室"),QStringLiteral(""));
        /*材料室的申请就是固定的*/
    }
    else
    {
        //失败就直接报告就行。
        ui->label->setText(QStringLiteral("请求失败,箱体单元未找到"));
    }
}
void BoxWindow::CmdSMFinished(CmdStateMachine * sm, int type, bool result, const QString &detial)
{
    if(type = SM_INSERT_REQUEST)
    {
        if(result)
        {
            ui->label->setText(QStringLiteral("请求已经成功")+detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","请求已经成功"+detial);
        }
        else
        {
            ui->label->setText(QStringLiteral("请求失败")+detial);
            FaderWidgetFactory::instance()->InfoPrint("命令执行","请求失败"+detial);
        }
    }

    /*无论返回结果是什么，都要删除sm*/
    sm->deleteLater();
}
void BoxWindow::UpdateListView()
{
    ui->listView->repaint();
}
void BoxWindow::CreateStockWindow(int buttonNum)
{
    //实例化子窗口不能指定父对象！，否则显示在父窗口里
    StockWindow * sw = StockWindowFactory::instance(0 ,buttonNum);
    sw->setWindowFlag(Qt::Window);
    sw->show();
}
void BoxWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
void BoxWindow::show_contextmenu(const QPoint& pos)
{
    int boxNum = ui->listView->model()->data(ui->listView->currentIndex(),Qt::DisplayRole).value<BoxData>().boxNum;
    if(cmenu)//保证同时只存在一个menu，及时释放内存
    {
        cmenu->deleteLater();
        cmenu = nullptr;
    }

//    qDebug()<<ui->listView->model()->data(ui->listView->currentIndex(),Qt::DisplayRole).value<BoxData>().boxNum;
    if(!((ui->listView->selectionModel()->selectedIndexes()).empty()))
    {
        cmenu = new QMenu(ui->listView);
        QAction *configAction = cmenu->addAction("重新载入配置文件");
        QAction *resetAction = cmenu->addAction("复位");
        connect(configAction, SIGNAL(triggered(bool)),
                this, SLOT(edit_menu1()));
        connect(resetAction, SIGNAL(triggered(bool)),
                this, SLOT(edit_menu2()));

        cmenu->exec(QCursor::pos());//在当前鼠标位置显示

        ui->listView->selectionModel()->clear();
    }
}
void BoxWindow::edit_menu1()
{
    int boxNum = ui->listView->model()->data(ui->listView->currentIndex(),Qt::DisplayRole).value<BoxData>().boxNum;
    /*如果包含有boxNum*/
    if(BoxTableCONFactory::instance()->IsBoxExist(boxNum))
    {
        if(SysConfigFactory::instance()->LoadJsonConfig(boxNum))
        {
            BoxTableCONFactory::instance()->SaveStock(boxNum,QString(SysConfigFactory::instance()->GetJsonConfig().toJson()));
        }
    }
}
void BoxWindow::edit_menu2()
{
    /*获得QListView的boxNum*/
    int boxNum = ui->listView->model()->data(ui->listView->currentIndex(),Qt::DisplayRole).value<BoxData>().boxNum;

    /*判断boxNum是不是存在MEM中*/
    if(RMSFactory()->boxMap.contains(boxNum))
    {

        /*找到boxNum对应的boxUnit,
         * 找到boxUnit对应的mirSchedule，
         * 然后复位*/
        RMSFactory()->boxMap.value(boxNum)->mirSchedule->ResetTheMirSchedule();
    }
}

BoxWindow::~BoxWindow()
{
    delete ui;
}
