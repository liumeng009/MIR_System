#include <QPainter>
#include <QDebug>
#include "MuItemDelegate.h"
#include "itemdef.h"

MuItemDelegate::MuItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}
void MuItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        painter->save();
        QVariant var = index.data(Qt::DisplayRole);
        DrawerData data = var.value<DrawerData>();

        // item 矩形区域
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width()-1);
        rect.setHeight(option.rect.height()-1);

        QPainterPath path;
        path.moveTo(rect.topRight());
        path.lineTo(rect.topLeft());
        path.quadTo(rect.topLeft(), rect.topLeft());
        path.lineTo(rect.bottomLeft());
        path.quadTo(rect.bottomLeft(), rect.bottomLeft());
        path.lineTo(rect.bottomRight());
        path.quadTo(rect.bottomRight(), rect.bottomRight());
        path.lineTo(rect.topRight());
        path.quadTo(rect.topRight(), rect.topRight());

        //优先渲染静态数据 brush填充
        if(data.checked == 1)
        {
            //painter->setPen(QPen(QColor("#e3e3e5")));
            painter->setBrush(QColor("#B0E0E6"));
            painter->drawPath(path);
        }
        else
        {
            //painter->setPen(QPen(QColor("#ebeced")));
            painter->setBrush(QColor("#e3e3e5"));
            painter->drawPath(path);
        }

        // 鼠标悬停或者选中时改变背景色
//        if (option.state.testFlag(QStyle::State_MouseOver)) {
//            painter->setPen(QPen(QColor("#ebeced")));
//            painter->setBrush(QColor("#ebeced"));
//            painter->drawPath(path);
//        }
        if (option.state.testFlag(QStyle::State_Selected)) {
            painter->setPen(QPen(Qt::green, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
            //painter->setBrush(QColor("#e3e3e5"));
            painter->drawPath(path);
        }


        // 绘制图片，歌手，数量位置区域
        QRectF iconRect = QRect(rect.left()+5, rect.top()+5, 40, 40);
        QRectF singerRect = QRect(iconRect.right()+5, iconRect.top(), rect.width()-10-iconRect.width(), 20);
        QRectF songNbRect = QRect(singerRect.left() - 40, singerRect.bottom(), rect.width(), 20);

        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Microsoft Yahei", 10));


        painter->setPen(QPen(Qt::gray));
        //painter->drawText(songNbRect, itemData.songsNb);
        painter->drawText(songNbRect, "仓格"+QString::number(data.drawerNum) +QStringLiteral(":")+ data.categoryName);

        painter->restore();
    }
}

QSize MuItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), 50);
}
