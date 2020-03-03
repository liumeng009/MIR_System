#include "excelcon.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>


ExcelCON::ExcelCON(QObject *parent) : QObject(parent)
{

}
bool ExcelCON::createExcelFile(const QString &fileName)
{
    QXlsx::Document xlsx;

    xlsx.write("A1", "Hello Qt!");

    QMessageBox::warning(NULL,"show path",fileName);

    qDebug()<<xlsx.saveAs("./test.xlsx");

    return 0;
}

