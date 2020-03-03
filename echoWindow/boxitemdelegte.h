#ifndef BoxItemDelegte_H
#define BoxItemDelegte_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPushButton>
#include <QPoint>
#include <QStringList>
#include <QScopedPointer>
#include <QMouseEvent>
#include <QPainter>
#include "boxmodel.h"


class BoxItemDelegte : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BoxItemDelegte(QWidget *parent = nullptr);

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
    void ButtonClicked(int num);
    void RequstButtonClicked(int num);

private:
    QPoint m_mousePoint;  // 鼠标位置
    QScopedPointer<QPushButton> m_pOpenButton;
    QScopedPointer<QPushButton> m_pDeleteButton;
    QStringList m_list;
    int m_nSpacing;  // 按钮之间的间距
    int m_nWidth;  // 按钮宽度
    int m_nHeight;  // 按钮高度
    int m_nType;  // 按钮状态-1：划过 0: 无动作 2：按下
};

#endif // BoxItemDelegte_H
