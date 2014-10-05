#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFile>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QTime>
#include <QTimer>

#ifdef LIBPKGSRC
#include <PkgSrc/pkgsrc.h>
#endif

#ifdef LIBDRACOPKG
#include <DracoPKG/dracopkg.h>
#endif

#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void initApp();
    void trayActivated();
    void catGen();
    //void installedPackagesGenTree();
    void addPackageToQueue(QString package, QString category, QString options);
    void delPackageFromQueue();
    void queueStart();
    void queueFinished(int status);
    void queueStop();
    void queueRead(QString data);
    void queueRetry();
    void on_queue_customContextMenuRequested();
    void on_cat_itemClicked(QListWidgetItem *item);
    void on_pkg_itemClicked(QListWidgetItem *item);
    void on_options_itemActivated(QTreeWidgetItem *item);
    void on_pushButton_clicked();
    void packageOptions(QStringList options);
    void packageDepends(QStringList depends);
    void pkgsrcDownloadStatus(qint64 start, qint64 end);
    void pkgsrcDownloadFinished(int status);
    void bootstrapExtractFinished(int status);
    void bootstrapExtractRead();
    void bootstrapMakeFinished(int status);
    void bootstrapMakeRead(QString data);
    bool bootstrapCheck();
    //void pkgCleanFinished(int status);
    void pkgsrcPkgNameFinished(QString name);
    void pkgsrcPkgVersionFinished(QString version);
    void pkgsrcPkgVulnCheckFinished(QStringList result);
    bool pkgsrcPkgInstalled(QString name);
    void pkgsrcInstalledFinished(QStringList packages);
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionAbout_pkgsrc_triggered();
    void on_actionAbout_Qt_triggered();
    void on_pkgVuln_itemDoubleClicked(QTreeWidgetItem *item);
    void on_pkgBox_currentChanged(int index);
    void genPkgList(QString cat);
    void on_search_editingFinished();

    void on_installedPackagesTree_customContextMenuRequested();
    void delPackage();
    void delPackageLog(QString text);
    void delPackageDone(int result);

    void on_actionSync_triggered();
    void pkgsrcSyncLog(QString log);
    void pkgsrcSyncDone(int status);

    void readMKconf();
    void saveMKconf();

    void on_mkconfSave_clicked();

    void on_mkconfUndo_clicked();
    void queueRowUp();
    void queueRowDown();

    void checkUpdates();
private:
    Ui::MainWindow *ui;
    PkgSrc pkgsrc;
    QSystemTrayIcon *tray;
    QTime timer;
    int time_snapshot;
};

#endif // MAINWINDOW_H
