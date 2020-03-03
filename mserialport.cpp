#include "mserialport.h"
#include "table.h"
#include <stdio.h>
#include <QDebug>

MSerialPort * MSerialPortFactory::m_pInstance = nullptr;

MSerialPort::MSerialPort(QObject *parent) : QObject(parent),serial(new QSerialPort(this)),demoTimer(new QTimer())
{
    connect(serial, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));

    demoTimer = new QTimer();
    connect(demoTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

}
MSerialPort::~MSerialPort()
{
    qDebug()<<"serial close";
    if(serial->isOpen())
        serial->clear();
    serial->close();
    //不用 系统会自动析构 serial->deleteLater();
}
QSerialPort * MSerialPort::GetSerial()
{
    return serial;
}
bool MSerialPort::IsOpen()
{
    return true;
}
void MSerialPort::SerialWrite(QString txData)
{
    QByteArray txBa;

    txBa.resize(txData.length());
    for(int i = 0; i < txData.length(); i++)
        txBa.data()[i] = (txData.at(i).unicode());

    serial->write(txBa);
}

QString MSerialPort::SerialReadExchange(QByteArray rxData)
{
    result = rxData;
    qDebug()<<result;
}
/*ready read 信号触发*/
void MSerialPort::handleReadyRead()
{
    result.append(serial->readAll());

    //接收到就计时 400ms没有接收就算停止
    demoTimer->start(400);
}
/*定时器触发*/
void MSerialPort::handleTimeout()
{
    demoTimer->stop();
    //QString a = QString::fromLocal8Bit(result);
    //qDebug()<<"串口接收"+a;
    emit Upload(result);
    result.clear();
}

void MSerialPort::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug()<<"串口错误";
    }
}

