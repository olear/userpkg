#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::readConf()
{
    QFile MKconf(QDir::homePath()+"/pkg/etc/mk.conf"); // TODO remove hardcoded path
    if (MKconf.exists()) {
        if (MKconf.open(QIODevice::ReadOnly)) {
            QTextStream stream(&MKconf);
            ui->conf->clear();
            ui->conf->appendPlainText(stream.readAll());
        }
    }
}

void Preferences::writeConf()
{
    QFile MKconf(QDir::homePath()+"/pkg/etc/mk.conf"); // TODO remove hardcoded path
    if (MKconf.exists()) {
        if (MKconf.open(QIODevice::WriteOnly)) {
            QTextStream stream(&MKconf);
            stream << ui->conf->toPlainText();
        }
        readConf();
    }
}

void Preferences::on_read_clicked()
{
    readConf();
}

void Preferences::on_save_clicked()
{
    writeConf();
    this->close();
}

void Preferences::on_close_clicked()
{
    this->close();
}
