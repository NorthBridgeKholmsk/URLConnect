#ifndef PASSWORKAPI_H
#define PASSWORKAPI_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QTimer>
#include <QEventLoop>
#include <QMap>
#include "../Logger/logger.h"

class PassworkAPI: public QObject{
public:
    explicit PassworkAPI(const QString& _apiKey, const QString& _hostname, const QString& _protocol);
    //explicit PassworkAPI(const QString& _apiKey, const QString& _passid);

    QString getLogin();
    QString getPass();

private:    
    QString apiKey;
    QString hostname;
    QString passid;
    QJsonObject response;

    QString getToken(const QJsonObject& response);
    QJsonObject sendResquest(const QString& method, const QMap<QString,QString>& headerList, const bool& isGetRequest, const QString& data="");
};

#endif // PASSWORKAPI_H
