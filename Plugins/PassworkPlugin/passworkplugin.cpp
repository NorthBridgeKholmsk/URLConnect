#include "passworkplugin.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>
#include <QMap>
#include <QUrl>

namespace {

//Синхронный HTTP-запрос к API Passwork с таймаутом 20 секунд.
//Возвращает разобранный JSON (пустой объект при ошибке сети или таймауте).
QJsonObject sendRequest(QNetworkAccessManager* manager, const QString& method,
                        const QMap<QString,QString>& headerList, bool isGetRequest, const QString& data = ""){
    QJsonObject result;
    QNetworkRequest request(QUrl("https://passwd.most.lan/api/v4" + method));
    for (auto it = headerList.constBegin(); it != headerList.constEnd(); ++it){
        request.setRawHeader(it.key().toLocal8Bit(), it.value().toLocal8Bit());
    }

    QNetworkReply *reply;
    if (isGetRequest){
        reply = manager->get(request);
    } else {
        reply = manager->post(request, data.toLocal8Bit());
    }

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(20000);
    loop.exec();

    if (timer.isActive()){
        timer.stop();
        QString jsonString = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
        result = document.object();
        if (reply->error() != QNetworkReply::NoError){
            qCritical() << "Возникла ошибка при получении данных из Passwork. " + result["data"].toObject()["error"].toString();
        }
    } else {
        QObject::disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        reply->abort();
    }
    reply->deleteLater();
    return result;
}

QString getToken(const QJsonObject& response){
    return response["data"]["token"].toString();
}

} // namespace

PassworkPlugin::PassworkPlugin(QObject *parent) : QObject(parent), m_manager(new QNetworkAccessManager(this)){
}

bool PassworkPlugin::getCredentials(const QString& apiKey, const QString& hostname,
                                    const QString& protocol, QString& login, QString& pass){
    //1. Авторизация — получение токена
    QMap<QString,QString> headerAuth = {{"Accept","application/json"}};
    QString token = getToken(sendRequest(m_manager, "/auth/login/" + apiKey, headerAuth, false));
    if (token.isEmpty()){
        qCritical() << "Passwork: не удалось получить токен авторизации";
        return false;
    }

    //2. Поиск пароля по имени хоста и тегам протокола/admin
    QMap<QString,QString> headerFindPass = {{"Accept","application/json"},{"Passwork-Auth",token}};
    QJsonObject response = sendRequest(m_manager, "/passwords/search", headerFindPass, false, "{\"query\":\"" + hostname + "\"}");

    QString tagProtocol = protocol;
    QString passid;
    QJsonArray passwds = response["data"].toArray();
    for (int i = 0; i < passwds.size(); i++){
        QJsonArray tags = passwds[i].toObject()["tags"].toArray();
        if (tags.contains(tagProtocol) && tags.contains("admin")){
            passid = passwds[i].toObject()["id"].toString();
            break;
        }
    }
    if (passid.isEmpty()){
        qCritical() << "Passwork: пароль для хоста \"" + hostname + "\" не найден";
        return false;
    }

    //3. Получение пароля по id
    QMap<QString,QString> headerPass = {{"Accept","application/json"},{"Passwork-Auth",token}};
    response = sendRequest(m_manager, "/passwords/" + passid, headerPass, true);

    //4. Выход из сессии
    sendRequest(m_manager, "/auth/logout", headerPass, false);

    login = response["data"].toObject()["login"].toString();
    QByteArray cryptedPass = QByteArray::fromBase64(response["data"].toObject()["cryptedPassword"].toString().toLocal8Bit());
    pass = QString(cryptedPass);

    if (login.isEmpty() && pass.isEmpty()){
        qCritical() << "Passwork: не удалось получить логин/пароль";
        return false;
    }
    return true;
}
