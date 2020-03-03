#include "draweritem.h"

DrawerItem::DrawerItem(QWidget *parent, ApplyUnit *applyUnit) :
    QWidget(parent)
  ,layout(new QGridLayout(this))
{
    this->applyUnit = applyUnit;

    //划分单元格1000 * 1000 spacing = 0 紧密排布
    for(int i = 0; i < 100; i++)
    {
        layout->setColumnStretch(i,1);
        layout->setColumnMinimumWidth(i,1);
        layout->setRowStretch(i,1);
        layout->setRowMinimumHeight(i,1);
        layout->setSpacing(0);
    }

}
void DrawerItem::BuildCells(int drawerNum, int role)
{
    CellItem * pCell;

    this->drawerNum = drawerNum;
    //基于仓格，读取单元格信息，并实例化cells
    foreach(DrawerUnit * pDrawerUnit, applyUnit->DrawerUnitList)
    {
        if(pDrawerUnit->DrawerNum == drawerNum)
        {
            foreach(CellUnit * pCellUnit, pDrawerUnit->CellUnitList)
            {
                //创建一个cell控件
                pCell = new CellItem(this,applyUnit);
                //设置cell控件尺寸
//                pCell->setGeometry(
//                            pCellUnit->pos_leftup_Y,
//                            pCellUnit->pos_leftup_X,
//                            pCellUnit->widget_height,
//                            pCellUnit->widget_width);
                layout->addWidget(pCell,
                                  pCellUnit->pos_leftup_Y,
                                  pCellUnit->pos_leftup_X,
                                  pCellUnit->widget_height,
                                  pCellUnit->widget_width);
                qDebug()<<"build cell 3";
                pCell->specsLabel->setText(pCellUnit->specsName);
                pCell->countLabel->setText(QString::number(pCellUnit->increaseAmount));
                //告诉单元格widget 箱 仓 单
                pCell->SetIndex(applyUnit->boxNum,
                                drawerNum,
                                pCellUnit->CellNum,
                                pCellUnit->checked);
                pCell->SetRole(role);

                //记录
                cellItemList.append(pCell);
                connect(pCell,SIGNAL(clicked()),
                        this,SIGNAL(OneItemAdded()));
            }
            break;
        }
    }
}
void DrawerItem::UpdateDrawer()
{
    DrawerData data =  applyUnit->GetDrawerData(drawerNum);
    //data留作以后处理...
    foreach(CellItem * pCell , cellItemList)
    {
        pCell->UpdateCell();
    }
}
void DrawerItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load("./images/"+drawerPic);
    QPixmap fitPix = pix.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    qDebug()<<"./images/"+drawerPic;
    painter.drawPixmap(this->rect(),fitPix);
}
//void DrawerItem::resizeEvent(QResizeEvent *event)
//{
//    Q_UNUSED(event)
//    QImage image;
//    QPalette palette;
//    image.load("D:/aqt/tt.png");
//    palette.setBrush(this->backgroundRole(), QBrush(image.scaled(this->size())));
//    this->setPalette(palette);
//}
