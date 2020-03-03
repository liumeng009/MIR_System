#include "cellitem.h"

CellItem::CellItem(QWidget *parent, ApplyUnit * applyUnit) : QWidget(parent)
  ,applyUnit(applyUnit)
  ,specsLabel(new QLabel(this))
  ,countLabel(new QLabel(this))
  ,layout(new QGridLayout(this))
{
    setAttribute(Qt::WA_StyledBackground,true);

    //划分单元格1000 * 1000 spacing = 0 紧密排布
    for(int i = 0; i < 4; i++)
    {
        layout->setColumnStretch(i,1);
        layout->setRowStretch(i,1);
        layout->setSpacing(0);
    }

    /*相对位置*/
    layout->addWidget(specsLabel,1,0,2,4);
    layout->addWidget(countLabel,0,3,1,1);
    specsLabel->setStyleSheet("background:transparent; \
                              font-weight: bold; \
                              font-family : sans-serif;");
    countLabel->setStyleSheet("background:transparent; \
                              font-weight: bold;  \
                              font-family : sans-serif;");

}

void CellItem::SetRole(int role)
{
    this->role = role;

    if(role == 1)
    {
        if(checked == 1)
        {
            //初始颜色
            setStyleSheet("background-color: rgb(255, 100, 255)");
            //设置颜色
            SetStartColor("background-color: rgb(255, 100, 255)");
            SetEndColor("background-color: rgb(100, 255, 100)");
        }
        else
        {
            //初始颜色
            setStyleSheet("background-color: rgba(255, 255, 224, 50%)");
            //设置颜色
            SetStartColor("background-color: rgba(255, 255, 224, 50%)");
            SetEndColor("background-color: rgb(100, 255, 100)");
        }
    }
    else
    {
        //初始颜色
        setStyleSheet("background-color: rgba(255, 255, 224, 50%)");

        //setStyleSheet("background-color: rgb(255, 255, 255)");
        //设置颜色
        //按下颜色
        SetStartColor("background-color: rgba(255, 255, 224, 50%)");
        SetEndColor("background-color: rgb(100, 255, 255)");
    }

}
void CellItem::SetStartColor(QString styleColor)
{
    this->startStyleColor = styleColor;
}
void CellItem::SetEndColor(QString styleColor)
{
    this->endStyleColor = styleColor;
}
void CellItem::SetIndex(int indexBoxNum, int indexDrawerNum, int indexCellNum, int indexChecked)
{
    boxNum = indexBoxNum;
    drawerNum = indexDrawerNum;
    cellNum = indexCellNum;
    checked = indexChecked;
}
void CellItem::UpdateCell()
{
    CellData data = applyUnit->GetCellData(drawerNum,cellNum);
    specsLabel->setText(data.specsName);

    countLabel->setText(QStringLiteral("当前") + \
                        QString::number(data.estimateStockAmount) + \
                        QStringLiteral("(可补") + \
                        QString::number(data.supplyAmount) + QStringLiteral(")"));
}

CellItem::~CellItem()
{
}
