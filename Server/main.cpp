#include <QApplication>
#include "urlconnectserver.h"
#include "localserver.h"
#include "../Logger/logger.h"

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qInstallMessageHandler(Logger::messageHandler);
    qInfo() << "Запуск программы";
    URLConnectServer cs;
    LocalServer ls(QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Северный мост\\URLConnect\\settings", QSettings::NativeFormat).value("port").toString());
    return a.exec();
}
