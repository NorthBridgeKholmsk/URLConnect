#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include <windows.h>

class Updater : public QObject
{
public:
    explicit Updater(QObject *parent = nullptr);
    bool isLatestVersion();
    void downloadLatestVersion();
    void installLatestVersion();
    QString getLatestVersion();
private:
    QString getLatestVersionNumber();

    QNetworkAccessManager* manager;
    QString currentVersion = APP_VERSION;
    QString latestVersion;
};

#endif // UPDATER_H
