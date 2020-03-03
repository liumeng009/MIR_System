#ifndef FADERWIDGET_H
#define FADERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QDesktopServices>
#include <QToolButton>

class TitleWidget;

class FaderWidget :public QWidget
{
    Q_OBJECT

public:
    FaderWidget(QWidget * parent = nullptr);
    ~FaderWidget();

    void InfoPrint(QString title, QString content);
    void setMsg(QString title, QString content, QString work);
    void paintEvent(QPaintEvent *);
    void showAsQQ();
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    bool isEnter;
    double tran = 1.0;
    TitleWidget * titleW = nullptr;
    QLabel * content = nullptr;
    QTimer * timerShow = nullptr;
    QTimer * timerStay = nullptr;
    QTimer * timerClose = nullptr;
    QRect deskRect;
    QPoint normalPoint;

public slots:
    void myMove();
    void myStay();
    void myClose();
    void seeInfo();

};

class TitleWidget :public QWidget
{
    Q_OBJECT

public:
    TitleWidget(QWidget * parent = nullptr);
    ~TitleWidget();

    void paintEvent(QPaintEvent *);
    void setTitleText(QString title);

    QLabel * titleText = nullptr;
    QToolButton *  btnClose = nullptr;
signals:
    void myClose();

};


/*
 * 工厂说明：此类（即用即删）不建议作为父对象，chatClinet除外
*/
class FaderWidgetFactory
{
public:
    static FaderWidget * instance()
    {
        return new FaderWidget();
    }
};


#endif // FADERWIDGET_H
