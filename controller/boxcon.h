#ifndef BOXCON_H
#define BOXCON_H

#include <QObject>
#include <QHostAddress>
#include <QTimer>
#include "rms.h"
#include "schedule/cmdstatemachine.h"

class BoxCON : public QObject
{
    Q_OBJECT
public:
    explicit BoxCON(QObject *parent = nullptr);
    ~BoxCON();

    //单例依赖
    RMS * rms = nullptr;
    BingquTableCON * bingquTableCON = nullptr;

    //成员
    QTimer * releaseTimer = nullptr;
    CmdStateMachine * cmdStateMachine = nullptr;

    //成员函数
    void RequestInsert(int type, QString bingqu, QString requestContent);
    void RequestDelete(QString bingqu);
    void RequestStatusQuery(QString bingqu);
    void RequestStockQuery(QString bingqu);

signals:
    void CmdBoxFinished(int type, bool result, const QVariant detial, const QString detail);

public:

public slots:
    void CmdSMFinished(int type, bool result, const QString detial);
    void TimerFunc();
};

/*
 * 工厂说明：此类（即用即删）不建议作为父对象，chatClinet除外
*/
class BoxCONFactory
{
public:
    static BoxCON * instance(QObject *parent = nullptr)
    {
        return new BoxCON(parent);;
    }
};

#endif // BoxCON_H
