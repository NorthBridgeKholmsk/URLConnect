#include "adcontrolplugin.h"

ADControlPlugin::ADControlPlugin(QObject *parent)
    : QObject{parent}
{

}

QString ADControlPlugin::adcontrolRunApp(const QString &host, const QString &protocol, const QString &login){
    qInfo() << "Обработчик команд плагина ADcontrolPlugin получил данные для запуска";
    if (protocol == "adctl-domain"){
        return "runas /netonly /savecred /user:" + login + " \"mmc dsa.msc /server=" + host + "\"";
    }
    else{
        return "";
    }
}
