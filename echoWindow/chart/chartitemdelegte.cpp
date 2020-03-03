#include "chartitemdelegte.h"


ChartItemDelegte::ChartItemDelegte(QWidget *parent)
        : QStyledItemDelegate(parent),
        m_pOpenButton(new QPushButton()),
        m_pDeleteButton(new QPushButton()),
        m_nSpacing(5),
        m_nWidth(25),
        m_nHeight(20)
{
}
// 绘制按钮
void ChartItemDelegte::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{

    if(index.isValid())
    {
        painter->save();

//        ItemStatus status = (ItemStatus)(index.data(Qt::UserRole).toInt());

        QVariant variant = index.data(Qt::DisplayRole);
        ItemData data = variant.value<ItemData>();

        QStyleOptionViewItem viewOption(option);//用来在视图中画一个item

        //获得长方形的尺寸
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth( option.rect.width()-1);
        rect.setHeight(option.rect.height()-1);

        //QPainterPath画圆角矩形
        const qreal radius = 7;
        QPainterPath path;
        path.moveTo(rect.topRight() - QPointF(radius, 0));
        path.lineTo(rect.topLeft() + QPointF(radius, 0));
        path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
        path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
        path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
        path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
        path.lineTo(rect.topRight() + QPointF(0, radius));
        path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

        if(option.state.testFlag(QStyle::State_Selected))
        {
            painter->setPen(QPen(Qt::blue));
            painter->setBrush(QColor(229, 241, 255));
            painter->drawPath(path);
        }
        else if(option.state.testFlag(QStyle::State_MouseOver))
        {
            painter->setPen(QPen(Qt::green));
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }
        else{
            painter->setPen(QPen(Qt::gray));
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }

        //绘制数据位置
        QRect NameRect = QRect(rect.left() +10, rect.top()+10, rect.width()-30, 20);
        QRect circle = QRect(NameRect.right(), rect.top()+10, 10, 10);
        //QRect telRect = QRect(rect.left() +10, rect.bottom()-25, rect.width()-10, 20);
        QRect telRect = QRect(option.rect.left() + option.rect.width()-120, option.rect.top()+20, 40, 40);

        /*两个button*/
        QRect decorationRect = QRect(option.rect.left() + option.rect.width()-170, option.rect.top()+10, 40, 40);
        QRect decorationRect1 = QRect(option.rect.left() + option.rect.width()-80, option.rect.top()+10, 40, 40);

        QPixmap pixmap = QPixmap(":/image/plus.png");
        painter->drawPixmap(decorationRect,pixmap);     //画出删除按钮

        pixmap = QPixmap(":/image/minus.png");
        painter->drawPixmap(decorationRect1,pixmap);

//        switch (status) {
//        case S_RED:
//            painter->setBrush(Qt::red);
//            painter->setPen(QPen(Qt::red));
//            break;
//        case S_BLUE:
//            painter->setBrush(Qt::blue);
//            painter->setPen(QPen(Qt::blue));
//            break;
//        case S_YELLOW:
//            painter->setBrush(Qt::yellow);
//            painter->setPen(QPen(Qt::yellow));
//            break;
//        }

        painter->drawEllipse(circle);     //画圆圈


        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Times", 12, QFont::Bold));
        painter->drawText(NameRect,Qt::AlignLeft,data.categoryName + data.specsName); //绘制名字

        painter->setPen(QPen(Qt::gray));
        painter->setFont(QFont("Times", 10));
        painter->drawText(telRect,Qt::AlignLeft, QString::number(data.increaseMount)); //绘制电话

        painter->restore();
    }
}
QSize ChartItemDelegte::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(120,60);
}

bool ChartItemDelegte::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    //构造一个矩形区域 用于相应删除按钮的操作区域
    QRect decorationRect = QRect(option.rect.left() + option.rect.width()-170, option.rect.top()+10, 40, 40);
    QRect decorationRect1 = QRect(option.rect.left() + option.rect.width()-80, option.rect.top()+10, 40, 40);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);//将事件转换为鼠标事件

    //判断当前事件是鼠标按钮事件，并且鼠标位置是属于当前矩形范围内，就发送deleteItem信号
    if (event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
    {
        model->setData(index,QVariant(1),Qt::EditRole);
    }
    if (event->type() == QEvent::MouseButtonPress && decorationRect1.contains(mouseEvent->pos()))
    {
        model->setData(index,QVariant(2),Qt::EditRole);
    }

    //返回编辑事件
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
