#include "wizard.h"
#include "ui_wizard.h"

Wizard::Wizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Wizard)
{
    ui->setupUi(this);
}

Wizard::~Wizard()
{
    delete ui;
}

void Wizard::on_cancel_clicked()
{
    this->close();
}

void Wizard::on_save_clicked()
{
   /* int threads = 0;
    int gcc = 0;
    int branch = 0;
    QString options;
    if (!ui->threads->currentText().isEmpty())
        threads = ui->threads->currentText().toInt();
    if (ui->compiler->currentText()!="system")
        gcc = ui->compiler->currentText().toInt();
    if (ui->branch->currentText()=="current")
        branch = 1;
    if (!ui->options->text().isEmpty())
        options = ui->options->text();

    pkgsrc.initPkgsrc(threads,gcc,branch,options);
    QSettings settings;
    settings.beginGroup("global");
    settings.setValue("firstrun",1);
    settings.endGroup();
    settings.sync();
    this->close();*/
}
