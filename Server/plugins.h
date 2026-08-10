#ifndef PLUGINS_H
#define PLUGINS_H

#include <QDialog>
#include <QMap>
#include <QCheckBox>

namespace Ui {
class plugins;
}

class plugins : public QDialog
{
    Q_OBJECT

public:
    explicit plugins(QWidget *parent = nullptr);
    ~plugins();

private slots:
    void on_applyButton_clicked();

    void on_canselButton_clicked();

private:
    Ui::plugins *ui;
    QMap<QCheckBox*, QString> pluginList;
};

#endif // PLUGINS_H
