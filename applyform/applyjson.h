#ifndef APPLYJSON_H
#define APPLYJSON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

class ApplyJson : public QObject
{
    Q_OBJECT
public:
    explicit ApplyJson(QObject *parent = nullptr);

    QJsonObject  ApplyJsonMessage;

    QJsonObject QstringToJson(QString jsonString);
    QString JsonToQstring();
signals:

public slots:
};

class ApplyJsonFactory
{
public:
    static ApplyJson * instance(QObject *parent = nullptr)
    {
        return new ApplyJson(parent);
    }
};

#endif // APPLYJSON_H
