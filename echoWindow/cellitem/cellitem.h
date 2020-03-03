#ifndef CELLITEM_H
#define CELLITEM_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPoint>
#include <QHBoxLayout>
#include "applyform/applyform.h"

class CellItem : public QWidget
{
    Q_OBJECT
public:
    explicit CellItem(QWidget *parent = nullptr, ApplyUnit * applyUnit = nullptr);
    ~CellItem();

    //单例依赖
    ApplyUnit * applyUnit = nullptr;

    //成员
    int role;   //display 0/ edit 1
    int boxNum;
    int drawerNum;
    int cellNum;
    int checked;

    QGridLayout * layout = nullptr;
    QLabel * specsLabel = nullptr;
    QLabel * countLabel = nullptr;

    QPoint mousePos;
    QString startStyleColor = "background-color: rgb(0,0,0)";
    QString endStyleColor = "background-color: rgb(0,0,0)";

    void SetRole(int role = 0);
    void SetStartColor(QString styleColor);
    void SetEndColor(QString styleColor);
    void SetIndex(int boxNum, int drawerNum, int cellNum, int checked);
    void UpdateCell();
signals:

public slots:

};

#endif // CELLITEM_H
