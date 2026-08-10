#include "plugins.h"
#include "ui_plugins.h"

plugins::plugins(QWidget *parent) :QDialog(parent), ui(new Ui::plugins){
    ui->setupUi(this);

    pluginList.insert(ui->CheckBox_AD_control, ""); //Плагин управление доменами
}

plugins::~plugins(){
    delete ui;
}

void plugins::on_applyButton_clicked(){
    for (auto it = pluginList.begin(); it != pluginList.end(); ++it){
        if (it.key()->checkState()){
           //Скачивание и установка плагина
        }
    }
}

