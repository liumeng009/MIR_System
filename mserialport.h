#ifndef MSERIALPORT_H
#define MSERIALPORT_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>


class MSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit MSerialPort(QObject *parent = nullptr);
    ~MSerialPort();

    QTimer * demoTimer;
    QSerialPort * serial = nullptr;

    QByteArray result;

    void SerialWrite(QString txData);   //QString -> QbyteArray

    QSerialPort * GetSerial();
    bool IsOpen();
signals:
    void Upload(QByteArray result);
public slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError serialPortError);
    QString SerialReadExchange(QByteArray rxData);   //QbyteArray -> QString
};

/*单例工厂,一个系统只需要一个serial*/
class MSerialPortFactory
{
public:
    static MSerialPort * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new MSerialPort(parent);
        }
        return m_pInstance;
    }

    static void Release()
    {
        if (m_pInstance != NULL)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }
public:
    static MSerialPort * m_pInstance;
};

#endif // MSERIALPORT_H
