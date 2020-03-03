#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

/*   chatServer++++++>
 *               serverworker++++++>
 *                             json++++++>
 *                                        boxCON
 *                             json<-----(槽)
 *               serverworker<-----
 *   chatServer<------
*/

class QJsonObject;

class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString userName() const;
    void setUserName(const QString &userName);
    void sendJson(const QJsonObject &jsonData);
    void ProcessJson(QJsonObject jsonData);
signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);
public slots:
    void disconnectFromClient();
    void CmdBoxFinished(int type, bool result, const QVariant content, const QString detail);
private slots:
    void receiveJson();
public:
    QTcpSocket *m_serverSocket;
    QString m_userName;
    QString userBingqu;
    QTimer * releaseTimer = nullptr;
};

#endif // SERVERWORKER_H
