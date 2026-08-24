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

namespace Ui {
class plugins;
}

class plugins : public QDialog
{
    Q_OBJECT

public:
    explicit plugins(QWidget *parent = nullptr);
    QObject* loadPlugin(const QString& pathToPluginFile);
    QString getPluginFilePath(QCheckBox* key);
    ~plugins();

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
    void handleReply(QNetworkReply *reply);

    QMap<QCheckBox*, QString> pluginList;
    QNetworkAccessManager *m_nam;
};

#endif // PLUGINS_H
