#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT
    
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();
    
public slots:
    void readConf();
    void writeConf();
private slots:
    void on_read_clicked();
    void on_save_clicked();
    void on_close_clicked();
private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
