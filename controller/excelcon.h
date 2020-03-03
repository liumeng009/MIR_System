#ifndef EXCELCON_H
#define EXCELCON_H

#include <QObject>
#include "xlsxdocument.h"


class ExcelCON : public QObject
{
    Q_OBJECT
public:
    explicit ExcelCON(QObject *parent = nullptr);

    //依赖

    //成员
    bool createExcelFile(const QString &fileName);
signals:

public slots:
};

#endif // EXCELCON_H
