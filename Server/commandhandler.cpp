#include "commandhandler.h"
#include <QProcess>
#include <QApplication>
#include <QRegularExpression>
#include <QDialog>
#include "securestorage.h"
#include "totp.h"
#include "authmethoddialog.h"
#include <QUuid>

//Проверка корректности имени хоста/IP (защита от внедрения команд)
static bool isValidHost(const QString &host){
    static const QRegularExpression re("^[A-Za-z0-9.\\-:]+$");
    return !host.isEmpty() && host.size() <= 253 && re.match(host).hasMatch();
}

CommandHandler::CommandHandler(LocalServer& localserver){
    connect(&localserver, &LocalServer::dataReceived, this, &CommandHandler::runApp);
}

void CommandHandler::runApp(const QString &host, const QString &protocol, const QString &_hostname){
    qInfo() << "Обработчик команд получил данные для запуска";

    if (!isValidHost(host)){
        qCritical() << "Некорректное имя хоста, запуск отменён: \"" + host + "\"";
        return;
    }

    QString hostname = _hostname;
    QString login = "";
    QString pass = "";

    const bool isAuthProtocol = (protocol == "ssh") || (protocol == "ciscossh") || (protocol == "winbox");

    if (isAuthProtocol){
        //Диалог выбора метода аутентификации: ручной ввод / Passwork / RADIUS
        const bool passworkAvailable = QFile(Plugins::pluginFilePath("PassworkPlugin.dll")).exists();
        AuthMethodDialog dlg(hostname, passworkAvailable);
        if (dlg.exec() != QDialog::Accepted){
            qInfo() << "Выбор метода аутентификации отменён пользователем";
            return;
        }

        switch (dlg.method()){
        case AuthMethodDialog::Manual:
            login = dlg.login();
            pass = dlg.password();
            break;
        case AuthMethodDialog::Passwork: {
            //Ключ расшифровывается через DPAPI; при ошибке используется исходное значение
            QByteArray storedApiKey = QByteArray::fromBase64(QSettings().value("settings/apiKey").toByteArray());
            QByteArray plainApiKey = SecureStorage::unprotect(storedApiKey);
            if (plainApiKey.isEmpty()) plainApiKey = storedApiKey;
            QString pwHost = dlg.hostname();
            if (pwHost.isEmpty()) pwHost = hostname; //запасной вариант — имя хоста из URL
            QApplication::setOverrideCursor(Qt::WaitCursor);
            bool ok = getPassworkCredentials(plainApiKey, pwHost, protocol, login, pass);
            QApplication::restoreOverrideCursor();
            if (!ok){
                return;
            }
            break;
        }
        case AuthMethodDialog::Radius:
            getRadiusCredentials(protocol, login, pass);
            break;
        }
    }
    else if (!hostname.isEmpty()){
        //Прочие протоколы (например vnc): получение данных из Passwork по имени хоста
        if (protocol == "vnc"){
            QStringList vncParts = hostname.split('.');
            if (vncParts.size() > 1){
                hostname = vncParts.at(1);
            }
        }
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QByteArray storedApiKey = QByteArray::fromBase64(QSettings().value("settings/apiKey").toByteArray());
        QByteArray plainApiKey = SecureStorage::unprotect(storedApiKey);
        if (plainApiKey.isEmpty()) plainApiKey = storedApiKey;

        bool ok = getPassworkCredentials(plainApiKey, hostname, protocol, login, pass);
        QApplication::restoreOverrideCursor();
        if (!ok){
            return;
        }
    }

    QString program;
    QStringList args;
    bool launchAsAdmin = false;
    QString adminCommand;
    if (((protocol == "ssh") || (protocol == "ciscossh")) && exeExists("settings/sshAppPath")){
        program = QSettings().value("settings/sshAppPath").toString();
        const QString sshUseApp = QSettings().value("settings/sshUseApp").toString();
        if (!pass.isEmpty()){
            if (sshUseApp == "PuTTY"){
                args << "-ssh" << login + "@" + host << "-pw" << pass;
            }
            else if (sshUseApp == "MobaXterm"){
                if (protocol == "ciscossh"){
                    args << "-newtab" << ("sshpass -p '" + pass + "' ssh -legacy -c aes256-cbc -oKexAlgorithms=+diffie-hellman-group1-sha1 " + login + "@" + host);
                }
                else{
                    args << "-newtab" << ("sshpass -p '" + pass + "' ssh " + login + "@" + host);
                }
            }
            else {
                qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы";
                return;
            }
        }
        else{
            if (sshUseApp == "PuTTY"){
                args << "-ssh" << host;
            }
            else {
                qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы или индификатор пароля небыл найден";
                return;
            }
        }
    }
    else if (protocol == "rdp" && exeExists("settings/rdpAppPath")){
        program = QSettings().value("settings/rdpAppPath").toString();
        args << "-V:" + host;
    }
    else if (protocol == "vnc" && exeExists("settings/vncAppPath")){
        program = QSettings().value("settings/vncAppPath").toString();
        if (!pass.isEmpty()){
            args << "-host=" + host << "-password=" + pass;
        }
        else{
            args << "-host=" + host;
        }
    }
    else if (protocol == "winbox" && exeExists("settings/winboxNewAppPath")){
        program = QSettings().value("settings/winboxNewAppPath").toString();
        if (!pass.isEmpty()){
            args << host << login << pass;
        }
        else{
            args << host << "admin" << hostname;
        }
    }
    else if (protocol == "ie" && exeExists("settings/ieAppPath")){
        program = "wscript.exe";
        args << QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\StartIE.vbs" << "http://" + host;
    }
    else if (protocol == "ping"){
        launchConsoleCommand("Ping " + host, "ping " + host + " -t");
    }
    else if (protocol == "traceroute"){
        launchConsoleCommand("Traceroute " + host, "tracert -d " + host);
    }
    else if (protocol == "telnet") {
        program = QSettings().value("settings/sshAppPath").toString();
        const QString telnetUseApp = QSettings().value("settings/sshUseApp").toString();
        if (telnetUseApp == "PuTTY"){
            args << "-telnet" << host;
        }
        else if (telnetUseApp == "MobaXterm"){
            args << "-newtab" << ("telnet " + host);
        }
        else {
            qCritical() << "Выбранный SSH клиент не поддерживается данной версией программы";
            return;
        }
    }
    else if (protocol.startsWith("adctl")){
        SettingsWindow sw;
        QString filePath = Plugins::pluginFilePath("ADControlPlugin.dll");

        QPluginLoader adLoader(filePath);
        ADControlPluginInterface *m_adPlugin = qobject_cast<ADControlPluginInterface*>(adLoader.instance());

        if (!m_adPlugin){
            qWarning() << "Плагин не реализует интерфейс ADControlPluginInterface: " + adLoader.errorString();
            return;
        }
        adminCommand = m_adPlugin->adcontrolRunApp(sw, host, protocol);
        launchAsAdmin = true;
    }
    else {
        qCritical() << "Протокол \"" + protocol + "\" не поддерживается данной версией программы "
                                                  "или не найден исполнительный файл";
        return;
    }

    HWND console = GetConsoleWindow();
    if (console)
        ShowWindow(console,SW_SHOW);
    if (launchAsAdmin){
        runAsAdmin(adminCommand);
    }
    else if (!program.isEmpty()){
        qInfo() << "Запуск приложения:" << program << args;
        if (!QProcess::startDetached(program, args)){
            qCritical() << "Не удалось запустить приложение: " + program;
        }
    }
}

bool CommandHandler::getPassworkCredentials(const QByteArray& plainApiKey, const QString& hostname,
                                            const QString& protocol, QString& login, QString& pass){
    QPluginLoader pwLoader(Plugins::pluginFilePath("PassworkPlugin.dll"));
    PassworkPluginInterface *pwPlugin = qobject_cast<PassworkPluginInterface*>(pwLoader.instance());
    if (!pwPlugin){
        qCritical() << "Плагин Passwork не установлен или не загружен: " + pwLoader.errorString();
        return false;
    }
    bool ok = pwPlugin->getCredentials(QString::fromUtf8(plainApiKey), hostname, protocol, login, pass);
    if (!ok){
        qCritical() << "Не удалось получить данные из Passwork";
        return false;
    }
    return true;
}

void CommandHandler::getRadiusCredentials(const QString& protocol, QString& login, QString& pass){
    bool need2FACode = false;
    login = QSettings().value("settings/radiusLogin").toString();
    if (protocol == "ssh"){
        login += "-linux";
        need2FACode = !need2FACode;
    }
    else if (protocol == "winbox"){
        login += "-mk";
        need2FACode = !need2FACode;
    }
    else if (protocol == "ciscossh"){
        login += "-cisco";
        need2FACode = !need2FACode;
    }
    if (need2FACode){
        //Пытаемся сгенерировать второй фактор автоматически по ключу TOTP из настроек
        QByteArray storedTotpSecret = QByteArray::fromBase64(QSettings().value("settings/totpSecret").toByteArray());
        QByteArray plainTotpSecret = SecureStorage::unprotect(storedTotpSecret);
        if (plainTotpSecret.isEmpty()) plainTotpSecret = storedTotpSecret; //обратная совместимость с нешифрованными данными
        pass = TOTP::generate(QString::fromUtf8(plainTotpSecret));
        if (!pass.isEmpty()){
            qInfo() << "Второй фактор сгенерирован автоматически по ключу TOTP";
        }
        else{
            //Ключ не задан или некорректен — ручной ввод (предыдущее поведение)
            SettingsWindow sw;
            pass = QInputDialog::getText(sw.topLevelWidget(), "Ввод второго фактора", "Введите второй фактор из аутентификатора", QLineEdit::Normal, QString(), nullptr, Qt::WindowStaysOnTopHint);
        }
    }
}

bool CommandHandler::exeExists(const QString& keyReg){
    return QFile(QSettings().value(keyReg).toString()).exists();
}

QString CommandHandler::PathShielding(const QString &path){
    QString nativePath = QDir::toNativeSeparators(path);
    if (nativePath.contains(' ')){
        return "\"" + nativePath + "\"";
    }
    return nativePath;
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

void CommandHandler::launchConsoleCommand(const QString& title, const QString& command){
    //QProcess::startDetached + cmd /K не удерживает окно консоли при запуске из GUI-приложения
    //(окно открывается на мгновение и закрывается). Создаём временный .bat и запускаем его через
    //shell (ShellExecute) с явным cmd /K — так консольное окно надёжно создаётся и остаётся открытым.
    const QString batPath = QDir::temp().filePath(
        "urlconnect_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".bat");
    {
        QFile f(batPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)){
            qCritical() << "Не удалось создать временный файл: " + batPath;
            return;
        }
        QTextStream ts(&f);
        ts << "@echo off\r\n";
        ts << "title " << title << "\r\n";
        ts << command << "\r\n";
    }
    const QString params = "/K \"" + QDir::toNativeSeparators(batPath) + "\"";
    qInfo() << "Запуск консольной команды:" << params;
    HINSTANCE h = ShellExecuteW(NULL, L"open", L"cmd.exe",
        reinterpret_cast<const wchar_t*>(params.utf16()), NULL, SW_SHOW);
    if (reinterpret_cast<INT_PTR>(h) <= 32){
        qCritical() << "Не удалось запустить команду: " + batPath;
    }
}
