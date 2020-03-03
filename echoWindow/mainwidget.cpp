#include "mainwidget.h"
#include "ui_chartwidget.h"


ChartWidget * ChartWidgetFactory::m_pInstance = nullptr;

ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartWidget)
  ,chartForm(ChartFormFactory::instance(parent))
{
    ui->setupUi(this);

//    QStandardItemModel *pModel = new QStandardItemModel(this);

//    for (int i=0; i < 10; ++i)
//    {
//        QStandardItem *pItem = new QStandardItem;
//        pItem->setData(QVariant::fromValue(i), Qt::UserRole);
//        pModel->appendRow(pItem);
//    }

    ChartModel * pModel = new ChartModel(this);
    ChartItemDelegte *pItemDelegate = new ChartItemDelegte(this);

    ui->listView->setUpdatesEnabled(false);
    ui->listView->setModel(pModel);
    ui->listView->setItemDelegate(pItemDelegate);
    ui->listView->setResizeMode(QListView::Adjust);
    //ui->listView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->listView->setUpdatesEnabled(true);


    connect(ui->cancelButton,SIGNAL(clicked(bool)),
            this,SLOT(CancelFunc()));
    connect(ui->submitButton,SIGNAL(clicked(bool)),
            this,SLOT(SubmitFunc()));

    connect(this,SIGNAL(DataChanged()),
            pModel,SIGNAL(layoutChanged()));
}

void ChartWidget::UpdateTheView()
{
    emit DataChanged();
}

void ChartWidget::CancelFunc()
{
    //更新chartForm数据到applyForm
    chartForm->UpdateToApplyForm();
    //隐藏自己
    hide();
}

void ChartWidget::SubmitFunc()
{
    //更新chartForm数据到applyForm
    chartForm->UpdateToApplyForm();
    //applyform -> json
    //statecon发送申请
    IndicateWidgetFactory::instance()->DeleteWidget();
    IndicateWidgetFactory::instance()->BuildWidget();
}
void ChartWidget::SubmitFuncReply()
{
    //根据回复给出提示
    //如果成功，跳转到指示界面
    //如果失败，清空内容，跳转到申请界面，重新开始
}

ChartWidget::~ChartWidget()
{
    delete ui;
}


