#ifndef REPORTCON_H
#define REPORTCON_H

#include <QObject>
#include "xlsxdocument.h"

class RecordTableCON;
class RMS;

class ReportCON : public QObject
{
    Q_OBJECT
public:
    explicit ReportCON(QObject *parent = nullptr);

    //依赖
    RMS * rms = nullptr;
    RecordTableCON * recordTableCON = nullptr;
    QXlsx::Document xlsx;

    //成员
    void ExportAllReport(QString startDate,
                      QString endDate);
    void ExportReport(QString startDate,
                      QString endDate,
                      int boxNum);
signals:

public slots:
};

/*
 * 工厂说明：此类（即用即删）不建议作为父对象，chatClinet除外
*/
class ReportCONFactory
{
public:
    static ReportCON * instance(QObject *parent = nullptr)
    {
        return new ReportCON(parent);;
    }
};

#endif // REPORTCON_H
