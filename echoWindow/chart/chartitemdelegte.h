#ifndef CHARTITEMDELEGTE_H
#define CHARTITEMDELEGTE_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QPoint>
#include <QStringList>
#include <QScopedPointer>
#include <QMouseEvent>
#include <QToolTip>
#include <QPainter>
#include <QDebug>
#include <QVariant>
#include "chartmodel.h"
#include "itemdef.h"

class ChartItemDelegte : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChartItemDelegte(QWidget *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index);

//    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                          const QModelIndex &index) const override;

//    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
//    void setModelData(QWidget *editor, QAbstractItemModel *model,
//                      const QModelIndex &index) const override;

//    void updateEditorGeometry(QWidget *editor,
//        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void open(const QModelIndex &index);
    void RemoveOne(int boxNum, int drawerNum, int cellNum);
    void AddOne(const QModelIndex &index);
    void repaint();
private:
    QPoint m_mousePoint;  // 鼠标位置
    QScopedPointer<QPushButton> m_pOpenButton;
    QScopedPointer<QPushButton> m_pDeleteButton;
    QStringList m_list;
    int m_nSpacing;  // 按钮之间的间距
    int m_nWidth;  // 按钮宽度
    int m_nHeight;  // 按钮高度
    int m_nType;  // 按钮状态-1：划过 2：按下
};

#endif // CHARTITEMDELEGTE_H
