#include "reportcon.h"
#include "rms.h"
#include "sql/recordtablecon.h"
#include "rmsfactory.h"
#include "controller/excelcon.h"

ReportCON::ReportCON(QObject *parent) : QObject(parent)
  ,rms(RMSFactory())
  ,recordTableCON(RecordTableCONFactory::instance())
{

}
void ReportCON::ExportAllReport(QString startDate,
                  QString endDate)
{
    foreach (int key, rms->boxMap.keys()) {
        ExportReport(startDate,endDate,key);
    }
}
void ReportCON::ExportReport(QString startDate,
                  QString endDate,
                  int boxNum)
{
    QVector<QStringList> result;

    //获得结果(单个box的该时间段内的记录)
    result = recordTableCON->GetRecordResult(startDate,endDate,boxNum);

    //准备出库map 入库map
    QMap<QString,int> inForm;
    QMap<QString,int> outForm;

    //遍历result
    //根据result中决定是出库还是入库
    //如果是入库
        //根据内容解析出plyUnit
        //根据cell(check=1)的品类名+规格名 在入库map中寻找有没有这样的条目
            //如果有，那么就直接加上increasemenrt
            //如果没有，那么就新建一个条目

    //如果是出库
        //

    foreach (QStringList item, result) {
        QString type = item.at(4);              //补货 取货
        QString content = item.at(5);           //实际订单内容
        if(type == QStringLiteral("取货"))
        {
            ApplyJson contentJson;
            ApplyUnit contentUnit;
            contentJson.QstringToJson(content);
            contentUnit.FromJson(contentJson.ApplyJsonMessage);

            //获得unit中的有效条目集合
            QVector<ItemData> vector = contentUnit.GetChechedCells();
            //遍历集合
            foreach (ItemData item, vector) {
                if(inForm.contains(item.categoryName+item.specsName))
                {
                    outForm[item.categoryName+item.specsName] += item.increaseMount;
                }
                else {
                    outForm.insert(item.categoryName+item.specsName,item.increaseMount);
                }
            }
            //插入map
        }
        else if(type == QStringLiteral("补货"))
        {
            ApplyJson contentJson;
            ApplyUnit contentUnit;
            contentJson.QstringToJson(content);
            contentUnit.FromJson(contentJson.ApplyJsonMessage);

            //获得unit中的有效条目集合
            QVector<ItemData> vector = contentUnit.GetChechedCells();
            //遍历集合
            foreach (ItemData item, vector) {
                if(inForm.contains(item.categoryName+item.specsName))
                {
                    inForm.find(item.categoryName+item.specsName).value() += item.increaseMount;
                }
                else {
                    inForm.insert(item.categoryName+item.specsName,item.increaseMount);
                }
            }
        }
    }

    //将maps输出到excel文件 ：inForm outForm就是最后处理的结果
    //在此生成的是某箱体编号的出库单和入库单，对应excel的某个sheet

    //首先新建sheet
    xlsx.addSheet(QString("box %1").arg(boxNum));

    //填入数据
    int row = 1;
    xlsx.write(row,1,QStringLiteral("品类规格"));
    xlsx.write(row,2,QStringLiteral("补货数量"));
    row++;

    foreach (QString key, inForm.keys()) {
         xlsx.write(row,1,key);
         xlsx.write(row,2,inForm.value(key));
         row++;
    }

    xlsx.write(row,1,QStringLiteral("品类规格"));
    xlsx.write(row,2,QStringLiteral("取货数量"));
    row++;

    foreach (QString key, outForm.keys()) {
         xlsx.write(row,1,key);
         xlsx.write(row,2,outForm.value(key));
         row++;
    }

    //QMessageBox::warning(NULL,"show path",fileName);

    qDebug()<<xlsx.saveAs("./test.xlsx");
    QDesktopServices::openUrl(QUrl::fromLocalFile("./test.xlsx"));
}
