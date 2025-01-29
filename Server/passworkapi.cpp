#include "passworkapi.h"

PassworkAPI::PassworkAPI(const QString& _apiKey, const QString& _hostname, const QString& _protocol){
    apiKey = _apiKey;
    hostname = _hostname;

    QMap<QString,QString>headerAuth = {{"Accept","application/json"}};
    QString token = getToken(sendResquest("/auth/login/"+apiKey, headerAuth, false));

    QMap<QString,QString>headerFindPass = {{"Accept","application/json"},{"Passwork-Auth",token}};
    response = sendResquest("/passwords/search", headerFindPass, false, "{\"query\":\""+hostname+"\"}");
    QString passid;
    QJsonArray passwds = response["data"].toArray();
    for (int i = 0; i < passwds.size(); i++){
        QJsonArray tags = passwds[i].toObject()["tags"].toArray();
        QString protocol;
        _protocol == "old_winbox" ? protocol = "winbox" : protocol = _protocol;
        if (tags.contains(protocol) && tags.contains("admin")){
            passid = passwds[i].toObject()["id"].toString();
            break;
        }
    }

    QMap<QString,QString>headerPass = {{"Accept","application/json"},{"Passwork-Auth",token}};
    response = sendResquest("/passwords/"+passid, headerPass, true);

    sendResquest("/auth/logout", headerPass, false);
}

QJsonObject PassworkAPI::sendResquest(const QString& method, const QMap<QString,QString>& headerList, const bool& isGetRequest, const QString& data){
    QJsonObject result;
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request(QUrl("https://s-passwd-nb.kholmsk.ru/api/v4"+method));
    QMapIterator<QString,QString>header(headerList);
    while(header.hasNext()){
        header.next();
        request.setRawHeader(header.key().toLocal8Bit(), header.value().toLocal8Bit());
    }
    QNetworkReply *reply;
    if (isGetRequest){
        reply = manager->get(request);
    }
    else{
        reply = manager->post(request, data.toLocal8Bit());
    }

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(10000);
    loop.exec();
    if (timer.isActive()){
        timer.stop();
        if (reply->error() == QNetworkReply::NoError){
            QString jsonString = reply->readAll();
            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
            result = document.object();
        }
        else{
            QString jsonString = reply->readAll();
            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
            result = document.object();
            qCritical() << "Возникла ошибка при получении данных из Passwork. " + result["data"].toObject()["error"].toString();
        }
    }
    else{
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        reply->abort();
    }
    reply->deleteLater();
    return result;
}

QString PassworkAPI::getLogin(){
    return response["data"].toObject()["login"].toString();
}

QString PassworkAPI::getPass(){
    QByteArray cryptedPass = QByteArray::fromBase64(response["data"].toObject()["cryptedPassword"].toString().toLocal8Bit());
    return QString(cryptedPass);
}

QString PassworkAPI::getToken(const QJsonObject &response){
    return response["data"]["token"].toString();
}


