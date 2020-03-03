#include <QApplication>
#include "singleapplication.h"
#include "rms.h"
#include "table.h"
#include <QDebug>
#include <QByteArray>
#include <QtEndian>
#include "schedule/cmdstatemachine.h"
#include "faderwidget.h"
#include "sql/bingqutablecon.h"
#include "sql/boxtablecon.h"
#include "sql/yihutablecon.h"
#include "sql/recordtablecon.h"
#include "chatserver/chatserver.h"
#include "echoWindow/stockwindow.h"
#include "echoWindow/reportwindow.h"
#include "schedule/requestnode.h"

int main(int argc, char *argv[])
{
    //QApplication app(argc, argv);

    SingleApplication app(argc, argv);
    if(app.isRunning())
        return 0;

    QFile qss(":/qss/button.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    RMS * mainBody = new RMS(&app);   /*主体程序*/
    //MirDebug md;
    //md.show();


//    QImage img;

//    img.load("./image/background.jpg");

//    img.save("./image/background.png");

//    qDebug()<<"bingqu num"<<BingquTableCONFactory::instance()->GetBingquName(1);
//    qDebug()<<"stock"<<BoxTableCONFactory::instance()->GetStock(1);
//    qDebug()<<"finger"<<YihuTableCONFactory::instance()->SaveYihuFinger(1,"leon");
//    qDebug()<<"record"<<RecordTableCONFactory::instance()->InsertRecord(1,"中病区","content");


    return app.exec(); /*交出执行权，但是对象不释放！*/

}
