#ifndef BOXWINDOW_H
#define BOXWINDOW_H

#include <QWidget>
#include <QVariant>
#include "schedule/cmdstatemachine.h"

namespace Ui {
class BoxWindow;
}

class BoxModel;
class BoxWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BoxWindow(QWidget *parent = 0);
    ~BoxWindow();

    //依赖


    BoxModel * pModel = nullptr;
    QMenu * cmenu = nullptr;
    QTimer * timer = nullptr;

    void paintEvent(QPaintEvent *);
public slots:
    void UpdateListView();
    void show_contextmenu(const QPoint& pos);
    void CreateStockWindow(int buttonNum);
    void CreateConnectFunc();
    void RequestCall(int buttonNum);
    void CmdSMFinished(CmdStateMachine * sm, int type, bool result, const QString &detial);

    void edit_menu1();
    void edit_menu2();

    void keyPressEvent(QKeyEvent *event);
private:
    Ui::BoxWindow *ui;
};

#endif // BOXWINDOW_H
