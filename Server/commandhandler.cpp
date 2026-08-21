#include "commandhandler.h"


CommandHandler::CommandHandler(LocalServer& localserver){
    connect(&localserver, &LocalServer::dataReceived, this, &CommandHandler::runApp);
}

void CommandHandler::runApp(const QString &host, const QString &protocol, const QString &_hostname){
    qInfo() << "Обработчик команд получил данные для запуска";

    QString hostname = _hostname;
    QString login = "";
    QString pass = "";
    if (!hostname.isEmpty()){
        if (protocol == "vnc"){
            hostname = hostname.split('.')[1];
        }
        PassworkAPI psapi(QByteArray::fromBase64(QSettings().value("settings/apiKey").toByteArray()), hostname, protocol);
        login = psapi.getLogin();
        pass = psapi.getPass();
    }
    else{
        bool need2FACode = false;
        login = QSettings().value("settings/radiusLogin").toString();
        if (protocol == "ssh"){
            login += "-linux";
            need2FACode = !need2FACode;
        }
        else if ((protocol == "winbox") || (protocol == "old_winbox")){
            login += "-mk";
            need2FACode = !need2FACode;
        }
        else if (protocol == "ciscossh"){
            login += "-cisco";
            need2FACode = !need2FACode;
        }
        if (need2FACode){
            SettingsWindow sw;
            pass = QInputDialog::getText(sw.topLevelWidget(), "Ввод второго фактора", "Введите второй фактор из аутентификатора", QLineEdit::Normal,QString(), nullptr ,Qt::WindowStaysOnTopHint);
        }
    }

    QString command;
    if (((protocol == "ssh") || (protocol == "ciscossh")) && exeIsExsists("settings/sshAppPath")){
        command = "start /b " + PathShielding(QSettings().value("settings/sshAppPath").toString());
        if (!pass.isEmpty()){
            if (QSettings().value("settings/sshUseApp").toString() == "PuTTY"){
                command += " -ssh " + login + "@" + host + " -pw " + pass;
            }
            else if (QSettings().value("settings/sshUseApp").toString() == "MobaXterm"){
                if (protocol == "ciscossh"){
                    command += " -newtab \"sshpass -p '" + pass + "' ssh -legacy -c aes256-cbc -oKexAlgorithms=+diffie-hellman-group1-sha1 " + login + "@" + host + "\"" ;
                }
                else{
                    command += " -newtab \"sshpass -p '" + pass + "' ssh " + login + "@" + host + "\"" ;
                }
            }
            else {
                qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы";
                return;
            }
        }
        else{
            if (QSettings().value("settings/sshUseApp").toString() == "PuTTY"){
                command += " -ssh " + host;
            }
            else {
                qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы или индификатор пароля небыл найден";
                return;
            }
        }
    }
    else if (protocol == "rdp" && exeIsExsists("settings/rdpAppPath")){
        command = "start /b " + PathShielding(QSettings().value("settings/rdpAppPath").toString());
        command += " -V:" + host;
    }
    else if (protocol == "vnc" && exeIsExsists("settings/vncAppPath")){
        command = "start /b " + PathShielding(QSettings().value("settings/vncAppPath").toString());
        if (!pass.isEmpty()){
            command += " -host=" + host + " -password=" + pass + "";
        }
        else{
            command += " -host=" + host;
        }
    }
    else if (protocol == "winbox" && exeIsExsists("settings/winboxNewAppPath")){
        command = "start /b " + PathShielding(QSettings().value("settings/winboxNewAppPath").toString());
        if (!pass.isEmpty()){
            command += " " + host + " " + login + " " + pass + "";
        }
        else{
            command += " " + host + " admin " + hostname;
        }
    }
    else if (protocol == "old_winbox" && exeIsExsists("settings/winboxOldAppPath")){
        command = "start /b " + PathShielding(QSettings().value("settings/winboxOldAppPath").toString());
        if (!pass.isEmpty()){
            command += " " + host + " " + login + " " + pass + "";
        }
        else{
            command += " " + host + " admin " + hostname;
        }
    }
    else if (protocol == "ie" && exeIsExsists("settings/ieAppPath")){
        command = PathShielding(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\StartIE.vbs");
        command += "  http://" + host;
    }
    else if (protocol == "ping"){
        command = "start /b cmd /K ping " + host + " -t";
    }
    else if (protocol == "traceroute"){
        command = "start /b cmd /K tracert -d " + host;
    }
    else if (protocol == "telnet") {
        command = "start /b " + PathShielding(QSettings().value("settings/sshAppPath").toString());
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
    else if (protocol.startsWith("adctl")){
        SettingsWindow sw;
        plugins plugin;
        QString filePath = plugin.getPluginFilePath(plugin.ui->CheckBox_AD_control);

        ADControlPluginInterface *m_adPlugin = qobject_cast<ADControlPluginInterface*>(plugin.loadPlugin(filePath));

        QString pluginName = QFileInfo(filePath).fileName();
        pluginName.chop(4);
        if (!m_adPlugin){
            qWarning() << "Плагин " + pluginName + " не реализует интерфейс ADControlPluginInterface";
            return;
        }
        command = m_adPlugin->adcontrolRunApp(sw, host, protocol);
    }
    else {
        qCritical() << "Протокол \"" + protocol + "\" не поддерживается данной версией программы "
                                                  "или не найден исполнительный файл";
        return;
    }

    HWND console = GetConsoleWindow();
    if (console)
        ShowWindow(console,SW_SHOW);
    if (!command.isEmpty()){
        if (protocol.startsWith("adctl")){
            runAsAdmin(command);
            return;
        }
        system(command.toStdString().c_str());
    }
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

void CommandHandler::runAsAdmin(const QString& command) {
    if (command.isEmpty()) return;

    SHELLEXECUTEINFO sei = {};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"runas";
    sei.lpFile = L"cmd.exe";
    QString params = QString("/c \"%1\"").arg(command);
    std::wstring wParams = params.toStdWString();
    sei.lpParameters = wParams.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        qCritical() << "Не удалось запустить команду с правами администратора. Пользователь отклонил UAC или ошибка системы.";
    }
}
