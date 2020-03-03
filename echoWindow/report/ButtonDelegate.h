#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QItemDelegate>
#include <echoWindow/stockcontent/stockcontentwindow.h>

class ButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QObject *parent = 0);
    //重载了两个重要的纯虚函数！如下
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:

public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> m_btns;

};

#endif // BUTTONDELEGATE_H
