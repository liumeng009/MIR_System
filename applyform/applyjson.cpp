#include "applyjson.h"
#include <QDebug>

ApplyJson::ApplyJson(QObject *parent) : QObject(parent)
{

}
QJsonObject ApplyJson::QstringToJson(QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
    if(jsonDocument.isNull())
    {
        qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
        return QJsonObject();
    }
    ApplyJsonMessage = jsonDocument.object();
    return ApplyJsonMessage;
}

QString ApplyJson::JsonToQstring()
{
    return QString(QJsonDocument(ApplyJsonMessage).toJson());
}
