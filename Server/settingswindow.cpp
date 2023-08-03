#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :QDialog(parent), ui(new Ui::SettingsWindow){
    ui->setupUi(this);
    //Убираем кнопку закрытия окна, окно закрывается только кнопками "Сохранить" и "Отмена"
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);

    //Получаем настройки из реестра
    getSettingsFromRegistr();

    //Подключаем кнопкам выбора файлов соответствующий вызов функции findExeFile
    connect(ui->sshFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->sshAppPath, "Выберите исполняемый файл PuTTY");});
    connect(ui->rdpFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->rdpAppPath, "Выберите исполняемый файл клиента RDP");});
    connect(ui->vncFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->vncAppPath, "Выберите исполняемый файл клиента VNC");});
    connect(ui->winboxNewFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->winboxNewAppPath, "Выберите исполняемый файл Winbox новой версии");});
    connect(ui->winboxOldFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->winboxOldAppPath, "Выберите исполняемый файл Winbox старой версии");});
    connect(ui->ieFindButton, &QPushButton::clicked, this, [&](){findExeFile(ui->ieAppPath, "Выберите исполняемый файл Internet Explorer");});
    //Подключаем кнопке блокировки MS Edge запуск программы Edge Blocker
    connect(ui->edgeBlockRunButton, &QPushButton::clicked, this, [](){
        QString pathToEdgeBlocker = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\EdgeBlocker\\EdgeBlock.exe";
        system(pathToEdgeBlocker.toStdString().c_str());
    });
}

SettingsWindow::~SettingsWindow(){
    delete ui;
}

//Обработчик клика по кнопке "Отменить"
void SettingsWindow::on_cancelButton_clicked(){
    getSettingsFromRegistr();
    this->hide();
}

//Функция, получающая настройки из реестра и записывающая данные в соответствующие поля
void SettingsWindow::getSettingsFromRegistr(){
    QSettings settings;
    ui->autostartCB->setChecked(settings.value("settings/autostart", true).toBool());
    ui->portLine->setText(QString::number(settings.value("settings/port", 7070).toInt()));

    //Если получено название SSH клиента, которого нет в списке, то устанавливается значение по уполчанию - "PuTTY"
    if (ui->sshUseApp->findText(settings.value("settings/sshUseApp", "PuTTY").toString())){
        ui->sshUseApp->setCurrentText(settings.value("settings/sshUseApp", "PuTTY").toString());
    }
    else{
        ui->sshUseApp->setCurrentText("PuTTY");
    }
    ui->sshAppPath->setText(settings.value("settings/sshAppPath", "C:\\bin\\PuTTY.exe").toString());
    ui->rdpAppPath->setText(settings.value("settings/rdpAppPath", "C:\\Windows\\system32\\mstsc.exe").toString());
    ui->vncAppPath->setText(settings.value("settings/vncAppPath", "C:\\Program Files (x86)\\TightVNC\\tvnviewer.exe").toString());
    ui->winboxNewAppPath->setText(settings.value("settings/winboxNewAppPath", "C:\\bin\\winbox.exe").toString());
    ui->winboxOldAppPath->setText(settings.value("settings/winboxOldAppPath", "C:\\bin\\winbox_old.exe").toString());
    ui->ieAppPath->setText(settings.value("settings/ieAppPath", "C:\\Program Files\\Internet Explorer\\iexplore.exe").toString());
}

//Функция, открывающая окно, для выбора исполняемого файла и записывающая путь до выбранного файла в поле line
void SettingsWindow::findExeFile(QLineEdit *line, const QString& title){
    QString filePath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, title, "C:\\", "Exe (*.exe)"));
    line->setText(filePath);
}

//Обработчик клика по кнопке "Сохранить"
void SettingsWindow::on_saveButton_clicked(){
    setSettingsToRegistr();
    this->hide();
}

//Функция, записывающая настройки в реестр и применяющая их
void SettingsWindow::setSettingsToRegistr(){
    QSettings settings;
    settings.setValue("settings/autostart", ui->autostartCB->isChecked());
    //Если автозапуск включен, то программа добавляется в автозагрузку ОС, иначе удаляется оттуда
    if (ui->autostartCB->isChecked()){
        QSettings autorunSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        autorunSettings.setValue(QCoreApplication::applicationName(), QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        autorunSettings.sync();
    }
    else{
        QSettings autorunSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.remove(QCoreApplication::applicationName());
    }
    settings.setValue("settings/port", ui->portLine->text().toInt());
    settings.setValue("settings/sshUseApp", ui->sshUseApp->currentText());
    settings.setValue("settings/sshAppPath", ui->sshAppPath->text());
    settings.setValue("settings/rdpAppPath", ui->rdpAppPath->text());
    settings.setValue("settings/vncAppPath", ui->vncAppPath->text());
    settings.setValue("settings/winboxNewAppPath", ui->winboxNewAppPath->text());
    settings.setValue("settings/winboxOldAppPath", ui->winboxOldAppPath->text());
    settings.setValue("settings/ieAppPath", ui->ieAppPath->text());
}
