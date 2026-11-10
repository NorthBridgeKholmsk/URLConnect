#ifndef PLUGINS_H
#define PLUGINS_H

#include <QDialog>
#include <QSettings>
#include <QMap>
#include <QCheckBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QPluginLoader>
#include <QStandardPaths>

namespace Ui {
class plugins;
}

class Plugins : public QDialog
{
    Q_OBJECT

public:
    explicit Plugins(QWidget *parent = nullptr);
    QObject* loadPlugin(const QString& pathToPluginFile);
    //Возвращает путь к файлу плагина в каталоге данных приложения (без создания диалога)
    static QString pluginFilePath(const QString& fileName);
    QString getPluginFilePath(QCheckBox* key);
    QString getPluginFilePath(const QString &url);
    ~Plugins();

    Ui::plugins *ui;

private slots:
    void on_applyButton_clicked();
    void on_canselButton_clicked();
    void download(const QString &url);

signals:
    void downloadFinished(bool success, const QString &filePath);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void getPluginsFromRegistr();
    void handleReply(QNetworkReply *reply, const QString &url);

    QMap<QCheckBox*, QString> pluginList;
    QNetworkAccessManager *m_nam;
};

#endif // PLUGINS_H
