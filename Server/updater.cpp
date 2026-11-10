#include "updater.h"

//Сравнение версий формата "X.Y.Z". Возвращает -1, если a < b; 0, если равны; 1, если a > b
static int compareVersions(const QString& a, const QString& b){
    QStringList pa = a.trimmed().split('.');
    QStringList pb = b.trimmed().split('.');
    int size = qMax(pa.size(), pb.size());
    for (int i = 0; i < size; ++i){
        int va = (i < pa.size()) ? pa[i].toInt() : 0;
        int vb = (i < pb.size()) ? pb[i].toInt() : 0;
        if (va != vb) return (va < vb) ? -1 : 1;
    }
    return 0;
}

Updater::Updater(QObject *parent): QObject{parent}, manager(new QNetworkAccessManager){
    latestVersion = getLatestVersionNumber();
}

bool Updater::isLatestVersion(){
    //Считаем, что версия актуальна, если текущая не меньше последней (защита от "обновления" до более старой)
    return compareVersions(currentVersion, latestVersion) >= 0;
}

void Updater::downloadLatestVersion(){
    if (latestVersion[latestVersion.size()-1] == '\n'){
        latestVersion.chop(1);
    }
    QNetworkRequest request(QUrl("https://github.com/NorthBridgeKholmsk/URLConnect/releases/download/"+latestVersion+"/URLConnectInstall.exe"));
    QNetworkReply *reply = manager->get(request);
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
            QFile f(QDir::tempPath()+"\\URLConnect\\URLConnectInstall.exe");
            f.open(QFile::WriteOnly);
            f.write(reply->readAll());
        }
        else{
            qCritical() << "Не удалось скачать последнюю версию программы. " + reply->errorString();
        }
    }
    else{
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        reply->abort();
    }
    reply->deleteLater();
}

void Updater::installLatestVersion(){
    QString installerDir = QDir::tempPath()+"\\URLConnect\\URLConnectInstall.exe";
    HWND console = GetConsoleWindow();
    if (console)
        ShowWindow(console,SW_HIDE);
    system(QString("start " + installerDir).toStdString().c_str());
}

QString Updater::getLatestVersion(){
    return latestVersion;
}

QString Updater::getLatestVersionNumber(){
    QString version = APP_VERSION;
    QNetworkRequest request(QUrl("https://raw.githubusercontent.com/NorthBridgeKholmsk/URLConnect/master/version"));
    QNetworkReply *reply = manager->get(request);
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
            version = reply->readAll();
        }
        else{
            qCritical() << "Не удалось получить номер последней версии программы. " + reply->errorString();
        }
    }
    else{
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        reply->abort();
    }
    reply->deleteLater();
    if (version[version.size()-1] == '\n'){
        version.chop(1);
    }
    return version;
}
