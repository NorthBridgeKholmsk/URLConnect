#include "adcontrolplugin.h"

ADControlPlugin::ADControlPlugin(QObject *parent)
    : QObject{parent}
{

}

QString ADControlPlugin::adcontrolRunApp(const QDialog& parrent, const QString &host, const QString &protocol){
    qInfo() << "Обработчик команд плагина ADcontrolPlugin получил данные для запуска";

    if (protocol == "adctl-domain"){
        QString login = QInputDialog::getText(parrent.topLevelWidget(), "Ввод имени пользователя", "Введите имя администратора домена в формате domain\\login", QLineEdit::Normal, QString(), nullptr ,Qt::WindowStaysOnTopHint);
        //QString pass = QInputDialog::getText(parrent.topLevelWidget(), "Ввод пароля пользователя", "Введите пароль администратора " + login, QLineEdit::Password, QString(), nullptr ,Qt::WindowStaysOnTopHint);
        QString domain = login.section('\\', 0, 0);
        QString user = login.section('\\', 1);

        /*QString cmdKeySave = QString("cmdkey /add:%1 /user:%2 /pass:%3")
                                .arg(host)
                                .arg(login)
                                .arg(pass);
        system(cmdKeySave.toStdString().c_str());*/

        return "runas /netonly /user:" + login + " \"mmc dsa.msc /server=" + host + "\"";

        /*STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {0};
        QString cmdLine = QString("\"%1\" %2").arg("C:\\Windows\\System32\\mmc.exe").arg("dsa.msc /server=" + host);
        std::wstring wCmdLine = cmdLine.toStdWString();
        QString fullUser = domain.isEmpty() ? user : domain + "\\" + user;
        BOOL success = CreateProcessWithLogonW(
                fullUser.toStdWString().c_str(),   // пользователь
                NULL,                               // домен (можно NULL, если указан в user)
                pass.toStdWString().c_str(),        // пароль
                LOGON_NETCREDENTIALS_ONLY,          // флаг
                L"C:\\Windows\\System32\\mmc.exe",  // приложение
                (LPWSTR)wCmdLine.c_str(),           // командная строка
                CREATE_NEW_CONSOLE,                 // флаги
                NULL,                               // окружение
                NULL,                               // текущий каталог
                &si,
                &pi
            );
        if (!success) {
                qCritical() << "CreateProcessWithLogonW завершился с ошибкой: " << GetLastError();
                return "";
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return "";*/
    }
    else{
        return "";
    }
}
