#include "buttondelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include <qDebug>

ButtonDelegate::ButtonDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton* button = m_btns.value(index);
    //if (!button)去掉if判断 有可能找不到这个按钮吗？
    //{
    button = new QStyleOptionButton();
    button->rect = option.rect.adjusted(4, 4, -4, -4);
    button->text = "open stock";
    button->state |= QStyle::State_Enabled;

    (const_cast<ButtonDelegate *>(this))->m_btns.insert(index, button);
    //}去掉if 多此一举
    painter->save();
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());

    }
    painter->restore();
    QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
//    QItemDelegate::paint(painter, option, index);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index)) {
            m_btns.value(index)->state |= QStyle::State_Sunken;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index))
        {
            m_btns.value(index)->state &= (~QStyle::State_Sunken);

//            QDialog *d = new QDialog();
//            d->setGeometry(0, 0, 200, 200);
//            d->move(QApplication::desktop()->screenGeometry().center() - d->rect().center());
//            d->show();
            qDebug()<<model->data(index).toString();
            StockContentWindowFactory::instance()->DisplayFormString(index.data().toString());
            StockContentWindowFactory::instance()->show();
        }
    }
}
