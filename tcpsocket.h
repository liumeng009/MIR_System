#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "itemdef.h"
#include "frame.h"

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:
    qintptr handle;     //关键的handle
    TcpSocket();
    TcpSocket(qintptr handle);

    /*周期读缓冲区的定时器*/
    QTimer * demoTimer;

signals:

public slots:
    void DisconnectSocket(qintptr handle);
    void DisconnectProcess();
    void ReadMessage();
    void WriteMessage();
};

#endif // TCPSOCKET_H
