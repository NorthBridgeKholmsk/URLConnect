#include "authmethoddialog.h"
#include "ui_authmethoddialog.h"

AuthMethodDialog::AuthMethodDialog(const QString& defaultHostname, bool passworkAvailable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthMethodDialog)
{
    ui->setupUi(this);

    //Вариант Passwork доступен только при установленном плагине
    if (!passworkAvailable){
        ui->passworkRadio->setVisible(false);
    }
    else{
        ui->hostnameEdit->setText(defaultHostname);
    }

    connect(ui->manualRadio, &QRadioButton::toggled, this, &AuthMethodDialog::onMethodChanged);
    connect(ui->passworkRadio, &QRadioButton::toggled, this, &AuthMethodDialog::onMethodChanged);
    connect(ui->radiusRadio, &QRadioButton::toggled, this, &AuthMethodDialog::onMethodChanged);

    onMethodChanged(); //применяем начальное состояние (по умолчанию — ручной ввод)
}

AuthMethodDialog::~AuthMethodDialog(){
    delete ui;
}

void AuthMethodDialog::onMethodChanged(){
    const bool manual = ui->manualRadio->isChecked();
    const bool passwork = ui->passworkRadio->isVisible() && ui->passworkRadio->isChecked();
    ui->manualGroup->setVisible(manual);
    ui->passworkGroup->setVisible(passwork);
}

AuthMethodDialog::Method AuthMethodDialog::method() const{
    if (ui->passworkRadio->isVisible() && ui->passworkRadio->isChecked()) return Passwork;
    if (ui->radiusRadio->isChecked()) return Radius;
    return Manual;
}

QString AuthMethodDialog::login() const{
    return ui->loginEdit->text().trimmed();
}

QString AuthMethodDialog::password() const{
    return ui->passEdit->text();
}

QString AuthMethodDialog::hostname() const{
    return ui->hostnameEdit->text().trimmed();
}