#include "displaydebug.h"
#include "faderwidget.h"

DisplayDebug::DisplayDebug(QWidget *parent) : QMainWindow(parent)
{
    updateTimer = new QTimer(this);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(UpdateValue()));
    updateTimer->start(1000);  //每100ms执行一次

    connect(&btnGetMission,SIGNAL(clicked(bool)),this,SLOT(GetMissionFunc()));
    connect(&btnPostMissionQueue,SIGNAL(clicked(bool)),this,SLOT(PostMissionQueueFunc()));
    connect(&btnDeleteMissionQueue,SIGNAL(clicked(bool)),this,SLOT(DeleteMissionQueueFunc()));
    connect(&btnGetStatus,SIGNAL(clicked(bool)),this,SLOT(GetStatusFunc()));
    connect(&btnPutStatus,SIGNAL(clicked(bool)),this,SLOT(PutStatusFunc()));
    connect(&btnGetPosition,SIGNAL(clicked(bool)),this,SLOT(GetPositionFunc()));



    /*widget才能显示，显示基于widget操作*/
    centerWindow = new QWidget;

    page = new QTabWidget;      //page是tab对象
    this->setCentralWidget(page);

    layout1 = new QHBoxLayout;  /*GET/Missions*/

    urlText.setText("http://172.16.101.30/api/v2.0.0/"); /*初始默认*/
    urlLabel.setText(tr("url设置"));
    urlGuid.setText(tr("guid"));
    btnGetMission.setText(tr("GET/Missions"));
    layout1->addWidget(&urlLabel);
    layout1->addWidget(&urlText);
    layout1->addWidget(&urlGuid);
    layout1->addWidget(&urlGuidText);
    layout1->addWidget(&btnGetMission);
    layout1->setStretchFactor(&urlText,1);
    layout1->setStretchFactor(&urlGuidText,4);

    layout2 = new QHBoxLayout;  /*Post/MissionQueue*/

    messageLabel.setText(tr("message设置"));
    missionIdLabel.setText(tr("missionID设置"));
    para1Label.setText(tr("参数1设置"));
    pare2Label.setText(tr("参数2设置"));
    priorityLabel.setText(tr("priority设置"));
    btnPostMissionQueue.setText(tr("Post/MissionQueue"));
    btnDeleteMissionQueue.setText(tr("delete/MissionQueue"));
    btnGetMissionQueue.setText(tr("get/MissionQueue"));

    layout2->addWidget(&messageLabel);
    layout2->addWidget(&messageText);
    layout2->addWidget(&missionIdLabel);
    layout2->addWidget(&missionIdText);
    layout2->addWidget(&para1Label);
    layout2->addWidget(&para1Text);
    layout2->addWidget(&pare2Label);
    layout2->addWidget(&para2Text);
    layout2->addWidget(&priorityLabel);
    layout2->addWidget(&priorityText);
    layout2->addWidget(&btnPostMissionQueue);
    layout2->addWidget(&btnDeleteMissionQueue);
    layout2->addWidget(&btnGetMissionQueue);


    layout3 = new QHBoxLayout;  /*Get/Status*/

    positionLabel.setText(tr("position"));
    stateIdLabel.setText(tr("stateid"));
    stateTextLabel.setText(tr("stateText"));
    btnGetStatus.setText(tr("Get/Status"));

    layout3->addWidget(&positionLabel);
    layout3->addWidget(&positionText);
    layout3->addWidget(&stateIdLabel);
    layout3->addWidget(&stateIdText);
    layout3->addWidget(&stateTextLabel);
    layout3->addWidget(&stateTextText);
    layout3->addWidget(&btnGetStatus);

    layout4 = new QHBoxLayout;  /*Put/Status*/

    setStateIdLabel.setText(tr("stateID设置"));
    positionLabel_1.setText(tr("position"));
    stateIdLabel_1.setText(tr("stateID"));
    stateTextLabel_1.setText(tr("stateText"));
    btnPutStatus.setText(tr("Put/Status"));

    layout4->addWidget(&setStateIdLabel);
    layout4->addWidget(&setStateIdText);
    layout4->addWidget(&positionLabel_1);
    layout4->addWidget(&positionText_1);
    layout4->addWidget(&stateIdLabel_1);
    layout4->addWidget(&stateIdText_1);
    layout4->addWidget(&stateTextLabel_1);
    layout4->addWidget(&stateTextText_1);
    layout4->addWidget(&btnPutStatus);

    layout5 = new QHBoxLayout;      /*Get/Position*/

    positionGuidLabel.setText(tr("guid list"));
    btnGetPosition.setText(tr("Get/Position"));
    layout5->addWidget(&positionGuidLabel);
    layout5->addWidget(&positionGuidText);
    layout5->addWidget(&btnGetPosition);

    layout6 = new QVBoxLayout;
    layout6->addLayout(layout1);
    layout6->addLayout(layout2);
    layout6->addLayout(layout3);
    layout6->addLayout(layout4);
    layout6->addLayout(layout5);
    layout6->setStretchFactor(layout1,2);
    layout6->setStretchFactor(layout2,1);
    layout6->setStretchFactor(layout3,1);
    layout6->setStretchFactor(layout4,1);
    layout6->setStretchFactor(layout5,2);

    centerWindow->setLayout(layout6);

    page->addTab(centerWindow,"MIR HTTP");

    //sqlView = new QTableView;
    //         view->setModel(model);
    //         view->hideColumn(0); // don't show the ID
    //         view->show();
    //sqlView->show();
}

void DisplayDebug::UpdateValue()
{

}


void DisplayDebug::SetSqlModel(QSqlTableModel * demoModel)
{
    sqlView->setModel(demoModel);
    //view->hideColumn(0); // don't show the ID
    sqlView->show();
}

void DisplayDebug::SetSqlView(TableView * demoView)
{
    sqlView = demoView;
}
void DisplayDebug::SetSqlWidget(QWidget * demoWidget,QString tableName)
{
    page->addTab(demoWidget,tableName);
}
