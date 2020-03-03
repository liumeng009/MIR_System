#ifndef DRAWERITEM_H
#define DRAWERITEM_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "cellitem.h"
#include "itemdef.h"

class DrawerItem : public QWidget
{
    Q_OBJECT
public:
    explicit DrawerItem(QWidget *parent = nullptr,
                        ApplyUnit * applyUnit = nullptr);
    void paintEvent(QPaintEvent *event);    //背景图
    //void resizeEvent(QResizeEvent *event);

    //成员
    int drawerNum;
    int checked;
    QString drawerPic;
    QList <CellItem *> cellItemList;

    void BuildCells(int drawerNum, int role = 0);
    void UpdateDrawer();

    //依赖
    ApplyUnit * applyUnit = nullptr;

    //成员
    QGridLayout * layout = nullptr;
signals:
    void OneItemAdded();
public slots:
};

#endif // DRAWERITEM_H
