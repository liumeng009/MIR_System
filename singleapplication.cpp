#include "singleapplication.h"
#include <QLocalSocket>

SingleApplication::SingleApplication(int argc, char **argv):QApplication(argc,argv)
{
    _isRunning=false;

    QCoreApplication::setApplicationName("localserver");
    QString serverName=QCoreApplication::applicationName();

    QLocalSocket socket;
    socket.connectToServer(serverName);

    if(socket.waitForConnected(500))
    {
        QTextStream stream(&socket);
        QStringList args=QCoreApplication::arguments();

        if(args.count()>1)
        {
            stream<<args.last();
        }else
        {
            stream<<QString();
        }
        stream.flush();
        qDebug()<<"Connected server,program will quit";

        socket.waitForBytesWritten();

        /**
         *qApp->quit(); 此代码是用来退出事件循环的;在构造函数中,事件循环
         *尚未启动,因此就无法退出. 最好就是设置标志判断在外部判断
         */


        _isRunning=true;

        return;
    }

    qDebug()<<"Can't connect to server,build a server";
    server=new QLocalServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(newLocalConnection()));

    if(!server->listen(serverName))
    {
        //防止程序崩溃时,残留进程服务,移除之
        if(server->serverError()==QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName);
            server->listen(serverName);
        }
    }


}


void SingleApplication::newLocalConnection()
{
    QLocalSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;

    socket->waitForReadyRead(1000);

    //显示传入参数值
    QTextStream in(socket);
    QString vl;
    in>>vl;
    qDebug()<<"The value is: "<<vl;

    delete socket;
}

SingleApplication::~SingleApplication()
{
    if(server != NULL)
        server->deleteLater();
}


bool SingleApplication::isRunning()
{
    return _isRunning;
}
