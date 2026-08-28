#include "plugins.h"
#include "ui_plugins.h"

plugins::plugins(QWidget *parent) :QDialog(parent), ui(new Ui::plugins){
    ui->setupUi(this);
    m_nam = new QNetworkAccessManager(this);
    pluginList.insert(ui->CheckBox_AD_control, "https://github.com/NorthBridgeKholmsk/URLConnect/raw/refs/heads/master/Plugins/ADControlPlugin/ADControlPlugin.dll"); //Плагин управление доменами
    getPluginsFromRegistr();
}

QObject *plugins::loadPlugin(const QString &pathToPluginFile){
    QPluginLoader loader(pathToPluginFile);
    QObject* plugin = loader.instance();

    if (plugin){
        return plugin;
    }
    else{
        QString fileName = QFileInfo(pathToPluginFile).fileName();
        fileName.chop(4);
        qCritical() << "Плагин " + fileName + " не загружен: " << loader.errorString();
        return nullptr;
    }
}

QString plugins::getPluginFilePath(QCheckBox *key){
    QString fileName = QFileInfo(pluginList.value(key)).fileName();
    QString pluginsDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/plugins";

    return QDir(pluginsDirPath).absoluteFilePath(fileName);
}

QString plugins::getPluginFilePath(const QString &url){
    QString fileName = QFileInfo(url).fileName();
    QString pluginsDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/plugins";

    return QDir(pluginsDirPath).absoluteFilePath(fileName);
}

plugins::~plugins(){
    delete ui;
}

void plugins::on_applyButton_clicked(){
    for (auto it = pluginList.begin(); it != pluginList.end(); ++it){
        if (it.key()->checkState()){
           download(it.value());
           QSettings().setValue("plugins/"+QFileInfo(it.value()).baseName(), true);
        }
        else {
            QString path = getPluginFilePath(it.key());
            if (QFile(path).exists()){
                QFile().remove(path);
            }
            QSettings().setValue("plugins/"+QFileInfo(it.value()).baseName(), false);
        }
    }
    this->hide();
}


void plugins::on_canselButton_clicked(){
    qInfo() << "Окно настройки плагинов закрыто без сохранения настроек";
    this->hide();
}

void plugins::download(const QString &url){
    QNetworkRequest request(url);
    QNetworkReply *reply = m_nam->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, url]() {
        handleReply(reply, url);
    });
    connect(reply, &QNetworkReply::downloadProgress,
            this, &plugins::downloadProgress);
}

void plugins::getPluginsFromRegistr(){
    QSettings settings;
    QList<QCheckBox*>pluginCheckBoxes = this->findChildren<QCheckBox*>();
    for (QCheckBox* plugin : pluginCheckBoxes){
        QString pluginPath = QUrl(pluginList.value(plugin)).path();
        QString pluginName = QFileInfo(pluginPath).baseName();
        bool check = settings.value("plugins/" + pluginName, false).toBool();
        plugin->setChecked(check);
    }
    //ui->CheckBox_AD_control->setChecked(settings.value("plugins/" + QFileInfo(pluginList.value(ui->CheckBox_AD_control)).baseName(), true).toBool());
}

void plugins::handleReply(QNetworkReply *reply, const QString &url){
    QString filePath;
    bool success = false;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();

        QString fileName = QFileInfo(reply->url().path()).fileName();
        if (fileName.isEmpty()) {
            fileName = "plugin.dll";  // запасной вариант
        }

        QDir dir = QFileInfo(getPluginFilePath(url)).absolutePath();
                //QDir::current();
        if (!QDir(dir).exists()) {
            QDir().mkpath(dir.absolutePath());
        }
        //dir.cd("plugins");
        filePath = dir.absoluteFilePath(fileName);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();
            success = true;
            qInfo() << "Скачивание плагина " << filePath;
        } else {
            qCritical() << "Не удалось сохранить плагин " << filePath << ": " << file.errorString();
        }
    } else {
        qCritical() << "Не удалось скачать плагин " << filePath << ": " << reply->errorString();
    }

    reply->deleteLater();
    emit downloadFinished(success, filePath);
}

