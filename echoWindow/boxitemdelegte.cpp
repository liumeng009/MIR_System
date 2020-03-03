#include "boxitemdelegte.h"
#include "echoWindow/stockcontent/stockcontentwindow.h"
#include <QDebug>
#include "itemdef.h"

BoxItemDelegte::BoxItemDelegte(QWidget *parent)
        : QStyledItemDelegate(parent),
        m_pOpenButton(new QPushButton()),
        m_pDeleteButton(new QPushButton()),
        m_nSpacing(5),
        m_nWidth(25),
        m_nHeight(20)
{
}
// 绘制按钮
void BoxItemDelegte::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{

    if(index.isValid())
    {
        painter->save();

//        ItemStatus status = (ItemStatus)(index.data(Qt::UserRole).toInt());

        QVariant variant = index.data(Qt::DisplayRole);
        BoxData data = variant.value<BoxData>();

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
        else
        {
            painter->setPen(QPen(Qt::gray));
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }

        //绘制数据位置
        QRect NameRect = QRect(rect.left() +10, rect.top()+10, rect.width()-30, 20);
        QRect circle = QRect(NameRect.right(), rect.top()+10, 10, 10);
        QRect telRect = QRect(rect.left() +10, rect.bottom()-25, rect.width()-10, 20);

        painter->drawEllipse(circle);     //画圆圈

        //绘制按钮
        QRect DeleteRect = QRect(viewOption.rect.left() + 100, viewOption.rect.top(),48,48);
        QRect DeleteRect1 = QRect(viewOption.rect.left() + 200, viewOption.rect.top(),48,48);
        QRect DeleteRect2 = QRect(option.rect.left() + 300, option.rect.top()+ 5, 48, 48);
        QRect DeleteRect3 = QRect(option.rect.left() + 400, option.rect.top()+ 15, 100, 20); //电量

        if(m_nType == 2)
        {
            if(DeleteRect.contains(m_mousePoint))
            {
                QPixmap pixmap = QPixmap(":/image/button_press.png");
                painter->drawPixmap(DeleteRect,pixmap);     //画出删除按钮
            }
            else
            {
                QPixmap pixmap = QPixmap(":/image/button_close.png");
                painter->drawPixmap(DeleteRect,pixmap);     //画出删除按钮
            }

            if(DeleteRect1.contains(m_mousePoint))
            {
                QPixmap pixmap = QPixmap(":/image/cart_press.png");
                painter->drawPixmap(DeleteRect1,pixmap);     //画出删除按钮
            }
            else
            {
                QPixmap pixmap = QPixmap(":/image/cart_close.png");
                painter->drawPixmap(DeleteRect1,pixmap);     //画出删除按钮
            }

            if(DeleteRect2.contains(m_mousePoint))
            {
                QPixmap pixmap = QPixmap(":/image/stock.png");
                painter->drawPixmap(DeleteRect2,pixmap);     //画出删除按钮
            }
            else
            {
                QPixmap pixmap = QPixmap(":/image/stock.png");
                painter->drawPixmap(DeleteRect2,pixmap);     //画出删除按钮
            }
        }
        else
        {
            QPixmap pixmap = QPixmap(":/image/button_close.png");
            painter->drawPixmap(DeleteRect,pixmap);     //画出删除按钮

            pixmap = QPixmap(":/image/cart_close.png");
            painter->drawPixmap(DeleteRect1,pixmap);

            pixmap = QPixmap(":/image/stock.png");
            painter->drawPixmap(DeleteRect2,pixmap);
        }

        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Times", 12, QFont::Bold));
        painter->drawText(NameRect,Qt::AlignLeft,QStringLiteral("转运箱")+ QString::number(data.boxNum)); //绘制名字

        painter->setPen(QPen(Qt::gray));
        painter->setFont(QFont("Times", 10));
        painter->drawText(telRect,Qt::AlignLeft, QString::number(data.boxNum)); //绘制电话

        painter->setPen(QPen(Qt::black)); /*电量*/
        painter->setFont(QFont("Times", 10));
        painter->drawText(DeleteRect3,Qt::AlignLeft, QStringLiteral("电量:")+QString::number(data.battery_percentage)+QStringLiteral("%"));

        painter->restore();
    }
}
QSize BoxItemDelegte::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(240,60);
}

bool BoxItemDelegte::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    //构造一个矩形区域 用于相应删除按钮的操作区域
    QRect decorationRect = QRect(option.rect.left() + 100, option.rect.top(), 48, 48);
    QRect decorationRect1 = QRect(option.rect.left() + 200, option.rect.top(), 48, 48);
    QRect decorationRect2 = QRect(option.rect.left() + 300, option.rect.top()+ 5, 48, 48);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);//将事件转换为鼠标事件
    m_mousePoint = mouseEvent->pos();   //采集鼠标位置
    //判断当前事件是鼠标按钮事件，并且鼠标位置是属于当前矩形范围内，就发送deleteItem信号
    if (event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
    {
        m_nType = 2;
        model->setData(index,QVariant(1),Qt::EditRole);

        return true;    //通知paint重新刷新 非常重要
    }
    if (event->type() == QEvent::MouseButtonRelease && decorationRect.contains(mouseEvent->pos()))
    {
        m_nType = 0;
        model->setData(index,QVariant(1),Qt::EditRole);
        emit RequstButtonClicked(index.data().value<BoxData>().boxNum);
        return true;    //通知paint重新刷新    非常重要
    }

    if (event->type() == QEvent::MouseButtonPress && decorationRect1.contains(mouseEvent->pos()))
    {
        m_nType = 2;
        model->setData(index,QVariant(2),Qt::EditRole);

        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease && decorationRect1.contains(mouseEvent->pos()))
    {
        m_nType = 0;
        emit ButtonClicked(index.data().value<BoxData>().boxNum);
        model->setData(index,QVariant(2),Qt::EditRole);

        return true;    //通知paint重新刷新    非常重要
    }

    if (event->type() == QEvent::MouseButtonPress && decorationRect2.contains(mouseEvent->pos()))
    {
        m_nType = 2;
        model->setData(index,QVariant(2),Qt::EditRole);
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease && decorationRect2.contains(mouseEvent->pos()))
    {
        m_nType = 0;
        StockContentWindowFactory::instance()->DisplayBoxNum(index.data().value<BoxData>().boxNum);
        model->setData(index,QVariant(1),Qt::EditRole);
        return true;    //通知paint重新刷新    非常重要
    }

    //返回编辑事件
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
/*new*/
//QWidget *BoxItemDelegte::createEditor(QWidget *parent,
//    const QStyleOptionViewItem &/* option */,
//    const QModelIndex &/* index */) const
//{
//    qDebug()<<"创建11111111111111111111111111111111";
//    ChartItemWidget *editor = ChartItemWidgetFactory::instance(parent);
//    //editor->setFrame(false);
//    //editor->setMinimum(0);
//    //editor->setMaximum(100);
//    editor->show();

//    return editor;
//}
//void BoxItemDelegte::setEditorData(QWidget *editor,
//                                    const QModelIndex &index) const
//{
//    int value = index.model()->data(index, Qt::DisplayRole).toInt();

//    ChartItemWidget *spinBox = static_cast<ChartItemWidget*>(editor);
//    //spinBox->setValue(value);
//}
////! [2]

////! [3]
//void BoxItemDelegte::setModelData(QWidget *editor, QAbstractItemModel *model,
//                                   const QModelIndex &index) const
//{
//    ChartItemWidget *spinBox = static_cast<ChartItemWidget*>(editor);
//    //spinBox->interpretText();
//    //int value = spinBox->value();

//    //model->setData(index, value, Qt::EditRole);
//}
////! [3]

////! [4]
//void BoxItemDelegte::updateEditorGeometry(QWidget *editor,
//    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
//{
//    editor->setGeometry(option.rect);
//}



// 响应按钮事件 - 划过、按下
//bool BoxItemDelegte::editorEvent(QEvent* event,
//                                   QAbstractItemModel* model,
//                                   const QStyleOptionViewItem& option,
//                                   const QModelIndex& index)
//{
//    if (index.column() != FILE_OPERATE_COLUMN)
//        return false;
//    m_nType = -1;
//    bool bRepaint = false;
//    QMouseEvent *pEvent = static_cast<QMouseEvent *> (event);
//    m_mousePoint = pEvent->pos();
//    int nCount = m_list.count();
//    int nHalf = (option.rect.width() - m_nWidth * nCount - m_nSpacing * (nCount - 1)) / 2;
//    int nTop = (option.rect.height() - m_nHeight) / 2;
//    // 还原鼠标样式
//    QApplication::restoreOverrideCursor();
//    for (int i = 0; i < nCount; ++i)
//    {
//        QStyleOptionButton button;
//        button.rect = QRect(option.rect.left() + nHalf + m_nWidth * i + m_nSpacing * i,
//                            option.rect.top() + nTop,  m_nWidth, m_nHeight);
//        // 鼠标位于按钮之上
//        if (!button.rect.contains(m_mousePoint))
//            continue;
//        bRepaint = true;
//        switch (event->type())
//        {
//        // 鼠标滑过
//        case QEvent::MouseMove:
//        {
//            // 设置鼠标样式为手型
//            QApplication::setOverrideCursor(Qt::PointingHandCursor);
//            m_nType = 0;
//            QToolTip::showText(pEvent->globalPos(), m_list.at(i));
//            break;
//        }
//            // 鼠标按下
//        case QEvent::MouseButtonPress:
//        {
//            m_nType = 1;
//            break;
//        }
//            // 鼠标释放
//        case QEvent::MouseButtonRelease:
//        {
//            if (i == 0)
//            {
//                emit open(index);
//            }
//            else
//            {
//                emit deleteData(index);
//            }
//            break;
//        }
//        default:
//            break;
//        }
//    }
//    return bRepaint;
//}
