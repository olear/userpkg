#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QFile>

#ifdef LIBPKGSRC
#include <PkgSrc/pkgsrc.h>
#endif

#ifdef LIBDRACOPKG
#include <DracoPKG/dracopkg.h>
#endif

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = 0);
    ~Backend();
signals:
    void updateListResult(QStringList result);
public slots:
    void requestUpdateList();
private slots:
    void updateList();
private:
    PkgSrc pkgsrc;
    QThread t;
};

#endif // BACKEND_H
