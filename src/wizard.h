#ifndef WIZARD_H
#define WIZARD_H

#include <QDialog>
#include <QSettings>

#ifdef LIBPKGSRC
#include <PkgSrc/pkgsrc.h>
#endif

#ifdef LIBDRACOPKG
#include <DracoPKG/dracopkg.h>
#endif

namespace Ui {
class Wizard;
}

class Wizard : public QDialog
{
    Q_OBJECT
    
public:
    explicit Wizard(QWidget *parent = 0);
    ~Wizard();
    Ui::Wizard *ui;
private slots:
    void on_cancel_clicked();
    void on_save_clicked();
private:

    PkgSrc pkgsrc;
};

#endif // WIZARD_H
