#include "commandhandler.h"

CommandHandler::CommandHandler(LocalServer& localserver){
    connect(&localserver, &LocalServer::dataReceived, this, &CommandHandler::runApp);
}

void CommandHandler::runApp(const QString &host, const QString &protocol, const QString &idPass){
    qInfo() << "Обработчик команд получил данные для запуска";
    //Здесь будет код для получения логина и пароля из Passwd
    /*QString login = "admin";
    QString pass = "ReaOnlMem34";*/

    QString command;
    if (protocol == "ssh" && exeIsExsists("settings/sshAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/sshAppPath").toString());
        if (QSettings().value("settings/sshUseApp").toString() == "PuTTY"){
            //command += " -ssh " + login + "@" + host + " -pw " + pass;
            command += " -ssh " + host;
        }
        /*else if (QSettings().value("settings/sshUseApp").toString() == "MobaXterm"){
              command += " -newtab \"sshpass -p " + pass + " ssh " + login + "@" + host + "\"" ;
        }*/
        else {
            qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы";
            return;
        }
    }
    else if (protocol == "rdp" && exeIsExsists("settings/rdpAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/rdpAppPath").toString());
        command += " -V:" + host;
    }
    else if (protocol == "vnc" && exeIsExsists("settings/vncAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/vncAppPath").toString());
        //command += " -host=" + host + " -password=" + pass;
        command += " -host=" + host + " -password=599599";
    }
    else if (protocol == "winbox" && exeIsExsists("settings/winboxNewAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/winboxNewAppPath").toString());
        //command += " " + host + " " + login + " " + pass;
        command += " " + host + " admin " + idPass;
    }
    else if (protocol == "old_winbox" && exeIsExsists("settings/winboxOldAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/winboxOldAppPath").toString());
        //command += " " + host + " " + login + " " + pass;
        command += " " + host + " admin " + idPass;
    }
    else if (protocol == "ie" && exeIsExsists("settings/ieAppPath")){
        command = "start " + PathShielding(QSettings().value("settings/ieAppPath").toString());
        command += "  http://" + host;
    }
    else if (protocol == "ping"){
        command = "start cmd /K ping " + host + " -t";
    }
    else if (protocol == "traceroute"){
        command = "start cmd /K tracert -d " + host;
    }
    else if (protocol == "telnet") {
        command = "start " + PathShielding(QSettings().value("settings/sshAppPath").toString());
        if (QSettings().value("settings/sshUseApp").toString() == "PuTTY"){
            command += " -telnet " + host;
        }
        else if (QSettings().value("settings/sshUseApp").toString() == "MobaXterm"){
            command += " -newtab \"telnet " + host + "\"";
        }
        else {
            qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы";
            return;
        }
    }
    else {
        qCritical() << "Протокол \"" + protocol + "\" не поддерживается данной версией программы "
                                                  "или не найден исполнительный файл";
        return;
    }

    HWND console = GetConsoleWindow();
    if (console)
        ShowWindow(console,SW_HIDE);
    system(command.toStdString().c_str());
}

bool CommandHandler::exeIsExsists(const QString& keyReg){
    return QFile(QSettings().value(keyReg).toString()).exists();
}

QString CommandHandler::PathShielding(const QString &path){
    QStringList pathList = path.split('\\');
    for (QString& p : pathList){
        if (p.contains(' ')){
            p.insert(0, '"');
            p.push_back('"');
        }
    }
    return pathList.join('\\');
}
