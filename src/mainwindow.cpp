#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->progressBar);
    ui->progressBar->hide();
    //ui->pkg->setUniformItemSizes(true);
    //ui->cat->setUniformItemSizes(true);
    ui->pushButton->setEnabled(false);
    ui->queue->setHeaderHidden(false);

    tray = new QSystemTrayIcon(this);

    connect(&pkgsrc,SIGNAL(packageOptionsResult(QStringList)),this,SLOT(packageOptions(QStringList)));
    connect(&pkgsrc,SIGNAL(downloadFinished(int)),this,SLOT(pkgsrcDownloadFinished(int)));
    connect(&pkgsrc,SIGNAL(downloadStatus(qint64,qint64)),this,SLOT(pkgsrcDownloadStatus(qint64,qint64)));
    connect(&pkgsrc,SIGNAL(extractFinished(int)),this,SLOT(bootstrapExtractFinished(int)));
    connect(&pkgsrc,SIGNAL(extractStatus(QString)),this,SLOT(bootstrapExtractRead()));
    connect(&pkgsrc,SIGNAL(bootstrapFinished(int)),this,SLOT(bootstrapMakeFinished(int)));
    connect(&pkgsrc,SIGNAL(bootstrapStatus(QString)),this,SLOT(bootstrapMakeRead(QString)));
    connect(&pkgsrc,SIGNAL(bmakeFinished(int)),this,SLOT(queueFinished(int)));
    connect(&pkgsrc,SIGNAL(bmakeStatus(QString)),this,SLOT(queueRead(QString)));
    connect(&pkgsrc,SIGNAL(packageDependsResult(QStringList)),this,SLOT(packageDepends(QStringList)));
    //connect(&pkgsrc,SIGNAL(packageCleanFinished(int)),this,SLOT(pkgCleanFinished(int)));
    connect(&pkgsrc,SIGNAL(packagesInstalledResult(QStringList)),this,SLOT(pkgsrcInstalledFinished(QStringList)));
    connect(&pkgsrc,SIGNAL(packageVersionResult(QString)),this,SLOT(pkgsrcPkgVersionFinished(QString)));
    connect(&pkgsrc,SIGNAL(packageNameResult(QString)),this,SLOT(pkgsrcPkgNameFinished(QString)));
    connect(&pkgsrc,SIGNAL(packagesVulnsResult(QStringList)),this,SLOT(pkgsrcPkgVulnCheckFinished(QStringList)));
    //connect(&pkgsrc,SIGNAL(package))
    //connect(&pkgsrc,SIGNAL(packageRe))
    //connect(&pkgsrc,SIGNAL(packageRemove()))
    connect(&pkgsrc,SIGNAL(packageRemoveStatus(QString)),this,SLOT(delPackageLog(QString)));
    connect(&pkgsrc,SIGNAL(packageRemoveResult(int)),this,SLOT(delPackageDone(int)));

    connect(&pkgsrc,SIGNAL(pkgsrcReady()),this,SLOT(bootstrapCheck()));

    pkgsrc.initPkgsrc();

    /*if (bootstrapCheck())
    {
        catGen();
        pkgsrc.packagesInstalledRequest();
        pkgsrc.packagesVulnsRequest();
    }*/

    //this->setWindowTitle("UserPKG " + pkgsrc.branchVersion());

    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayActivated()));
    tray->setIcon(QIcon(":/files/tray.png"));
    tray->setToolTip("UserPKG");
    if (tray->isSystemTrayAvailable())
    {
        tray->show();
    }
}

void MainWindow::trayActivated()
{
        if(this->isHidden())
        {
            this->show();
        }
        else
        {
            this->hide();

        }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (tray->isVisible())
    {
        this->hide();
        event->ignore();
    }
    else
    {
        if (!pkgsrc.bmakeActive()&&!pkgsrc.bootstrapActive())
        {
            event->accept();
        }
        else
        {
            ui->statusBar->showMessage("Please cancel build before exiting");
            event->ignore();
        }
    }
}

MainWindow::~MainWindow()
{
        delete ui;
}

void MainWindow::catGen()
{
    ui->pkg->clear();
    ui->cat->clear();
    QStringList categories = pkgsrc.categoryList();
    for (int i = 0; i < categories.size(); ++i)
    {
        if (!categories.at(i).isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(categories.at(i));
            item->setIcon(QIcon(":/files/folder.png"));
            ui->cat->addItem(item);
        }
    }
}

void MainWindow::genPkgList(QString cat)
{
    ui->pkg->clear();
    QStringList packages = pkgsrc.packageList(cat,ui->search->text());
    for (int i = 0; i < packages.size(); ++i)
    {
        QString value = packages.at(i);
        QString package;
        QString category;
        if (value.contains("|"))
        {
            QStringList values = value.split("|",QString::SkipEmptyParts);
            if (!values.isEmpty())
            {
                category = values.takeFirst();
                if (!values.isEmpty())
                {
                    package = values.takeFirst();
                }
            }
        }
        else
        {
            package = value;
        }
        if (!package.isEmpty())
        {
            if (!pkgsrcPkgInstalled(package))
            {
                QListWidgetItem *newItem = new QListWidgetItem(package);
                newItem->setIcon(QIcon(":/files/package.png"));

                if (ui->search->text().isEmpty())
                {
                    newItem->setData(5,cat);
                }
                else
                {
                    if (!category.isEmpty())
                    {
                        newItem->setData(5,category);
                    }
                }

            /*if (!category.isEmpty())
            { // old lost feature from pre1, add description
                newItem->setData(3,category);
            }*/

                ui->pkg->addItem(newItem);
            }
        }
    }
}

void MainWindow::on_cat_itemClicked(QListWidgetItem *item)
{
    if (!item->text().isEmpty())
    {
        ui->search->clear();
        genPkgList(item->text());
    }
}

void MainWindow::on_pkg_itemClicked(QListWidgetItem *item)
{
    ui->options->clear();
    ui->pkgVersion->clear();
    ui->pkgName->clear();
    ui->pkgDepends->clear();

ui->pkgBox->setCurrentIndex(0);

   // if (!pkgsrcPkgName->isOpen())
     //   pkgsrcPkgNameExec(item->text(),item->data(5).toString());
    //if (!pkgsrcPkgVersion->isOpen())
      //  pkgsrcPkgVersionExec(item->text(),item->data(5).toString());

    pkgsrc.packageNameRequest(item->text(),item->data(5).toString());
    pkgsrc.packageVersionRequest(item->text(),item->data(5).toString());


    if (!ui->pushButton->isEnabled())
    {
        ui->pushButton->setEnabled(true);
    }


}



void MainWindow::on_pushButton_clicked()
{
    if (ui->pkg->currentRow()>-1)
    {
        QString options;
        int optionsCount = 0;
        if (ui->options->topLevelItemCount()>0)
        {
            while (optionsCount<ui->options->topLevelItemCount())
            {
                if (ui->options->topLevelItem(optionsCount)->data(0,3)=="+")
                    options.append(ui->options->topLevelItem(optionsCount)->text(0)+" ");
                if (ui->options->topLevelItem(optionsCount)->data(0,3)=="-")
                    options.append("-"+ui->options->topLevelItem(optionsCount)->text(0)+" ");
                optionsCount++;
            }
        }
        ui->options->clear();
        ui->pkgVersion->clear();
        ui->pkgName->clear();
        ui->pkgDepends->clear();
        ui->pushButton->setDisabled(true);
        addPackageToQueue(ui->pkg->currentItem()->text(),ui->pkg->currentItem()->data(5).toString(),options);
        delete ui->pkg->currentItem();
    }
}


void MainWindow::on_options_itemActivated(QTreeWidgetItem *item)
{
    if (item->data(0,3).toString()=="+")
    {
        item->setData(0,3,"-");
        item->setIcon(0,QIcon(":/files/remove.png"));
    }
    else {
        item->setData(0,3,"+");
        item->setIcon(0,QIcon(":/files/add.png"));
    }
}

void MainWindow::installedPackagesGenTree()
{
    ui->installedPackagesTree->clear();
    QDir pkgsrc(QDir::homePath() + "/userpkg/var/db/pkg");
    QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
    foreach (QFileInfo folder, pkgsrcList){
            if (folder.isDir()) {
                QFile Makefile(folder.filePath()+"/+COMMENT");
                if (Makefile.exists(folder.filePath()+"/+COMMENT"))
                {
                    QString pkgVersion = folder.fileName().mid(folder.fileName().lastIndexOf("-")).replace("-","");
                    QString pkgName = folder.fileName().left(folder.fileName().lastIndexOf("-"));
                    QTreeWidgetItem *newItem = new QTreeWidgetItem;
                    newItem->setText(0,pkgName);
                    newItem->setText(1,pkgVersion);
                    newItem->setIcon(0,QIcon(":/files/package.png"));
                    ui->installedPackagesTree->addTopLevelItem(newItem);
                }
        }
    }
}

void MainWindow::pkgsrcInstalledFinished(QStringList packages)
{
    ui->installedPackagesTree->clear();
    QStringList pkgList = packages;
    foreach(QString pkg,pkgList)
    {
        QStringList pkgInfo = pkg.split("|",QString::SkipEmptyParts);
        if (!pkgInfo.isEmpty())
        {
            QString pkgName;
            QString pkgVersion;
            QString pkgDesc;

            if (!pkgInfo.isEmpty())
            {
                pkgName = pkgInfo.takeFirst();
                if (!pkgInfo.isEmpty())
                {
                    pkgVersion = pkgInfo.takeFirst();
                    if (!pkgInfo.isEmpty())
                    {
                        pkgDesc = pkgInfo.takeFirst();
                    }
                }
            }
            if (!pkgName.isEmpty()&&!pkgVersion.isEmpty()&&!pkgDesc.isEmpty())
            {
                QTreeWidgetItem *newItem = new QTreeWidgetItem;
                newItem->setText(0,pkgName);
                newItem->setText(1,pkgVersion);
                newItem->setText(2,pkgDesc);
                newItem->setIcon(0,QIcon(":/files/queue-package.png"));
                ui->installedPackagesTree->addTopLevelItem(newItem);
            }
        }
    }
}

void MainWindow::on_actionQuit_triggered()
{
    if (tray->isVisible())
    {
        if (!pkgsrc.bmakeActive()&&!pkgsrc.bootstrapActive())
        {
            qApp->quit();
        }
        else
        {
            ui->statusBar->showMessage("Please cancel build before exiting");
        }
    }
    else
    {
        this->close();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About UserPKG "+QApplication::applicationVersion(),"<h3>UserPKG "+QApplication::applicationVersion()+"</h3>Unprivileged package manager.<br><br>UserPKG Copyright &copy; 2014 Ole Andre Rodlie. All rights reserved.<br><br>UserPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.<br><br>UserPKG is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br><br>You should have received a copy of the GNU General Public License version 2 along with UserPKG.  If not, see http://www.gnu.org/licenses/.<br><br>Visit <a href='http://dracolinux.org'>DracoLinux.org</a> for more information. Comments, bugs etc goes to <a href='mailto:olear@dracolinux.org'>olear@dracolinux.org</a>.");
}

void MainWindow::on_actionAbout_pkgsrc_triggered()
{
    QMessageBox::about(this,"About pkgsrc "+pkgsrc.branchVersion(),"<h3>pkgsrc "+pkgsrc.branchVersion()+"</h3>pkgsrc is a framework for building third-party software on NetBSD and other UNIX-like systems, currently containing over 12000 packages. It is used to enable freely available software to be configured and built easily on supported platforms.<br><br>Copyright &copy; 1994-2014 The NetBSD Foundation, Inc. All rights reserved.<br><br>NetBSD is a registered trademark of the NetBSD Foundation, Inc.<br><br>Visit <a href='http://www.pkgsrc.org'>pkgsrc.org</a> for more information");
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}


void MainWindow::on_pkgVuln_itemDoubleClicked(QTreeWidgetItem *item)
{
    QDesktopServices::openUrl(QUrl::fromUserInput(item->data(0,3).toString()));
}

void MainWindow::packageOptions(QStringList options)
{
    if (!ui->packagesTab->isEnabled())
    {
        ui->packagesTab->setEnabled(true);
    }

    if (!options.isEmpty())
    {
        for (int i = 0; i < options.size(); ++i)
        {
            QString value = options.at(i);
            QTreeWidgetItem *item = new QTreeWidgetItem;
            if (value.startsWith("+"))
            {
                item->setText(0,value.mid(1));
                item->setIcon(0,QIcon(":/files/add.png"));
                item->setData(0,3,"+");
            }
            if (value.startsWith("-"))
            {
                item->setText(0,value.mid(1));
                item->setIcon(0,QIcon(":/files/remove.png"));
                item->setData(0,3,"-");
            }
            if (!item->text(0).isEmpty()&&!item->data(0,3).isNull())
            {
                ui->options->addTopLevelItem(item);
            }
        }
    }
    ui->statusBar->showMessage("Done",500);
}

void MainWindow::on_pkgBox_currentChanged(int index)
{
    QListWidgetItem *item = ui->pkg->currentItem();
    switch(index)
    {
    case 1:
        if (ui->pkgDepends->topLevelItemCount()==0)
        {
            if (item)
            {
                pkgsrc.packageDependsRequest(item->text(),item->data(5).toString());
                ui->statusBar->showMessage("Checking dependencies, please wait ...");
                ui->packagesTab->setEnabled(false);
            }
        }
        break;
    case 2:
        if (ui->options->topLevelItemCount()==0)
        {
            if (item)
            {
                pkgsrc.packageOptionsRequest(item->text(),item->data(5).toString());
                ui->statusBar->showMessage("Checking build options, please wait ...");
                ui->packagesTab->setEnabled(false);
            }
        }
        break;
    }
}

void MainWindow::on_search_editingFinished()
{
    if (!ui->search->text().isEmpty())
    {
        genPkgList("");
    }
    else
    {
        if (ui->cat->currentItem())
        {
            genPkgList(ui->cat->currentItem()->text());
        }
    }
}

void MainWindow::packageDepends(QStringList depends)
{
    if (!ui->packagesTab->isEnabled())
    {
        ui->packagesTab->setEnabled(true);
    }

    if (!depends.isEmpty())
    {
        for (int i = 0; i < depends.size(); ++i)
        {
            int index = depends.at(i).indexOf(":");
            if (!depends.at(i).left(index).isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem;
                item->setText(0,depends.at(i).left(index));
                ui->pkgDepends->addTopLevelItem(item);
            }
        }
    }
    ui->statusBar->showMessage("Done",500);
}

bool MainWindow::bootstrapCheck()
{ // TODO move to lib
    bool status = false;

    QString pkgProfile;
    pkgProfile.append("\n# UserPKG Integration [START]\n");
    pkgProfile.append("export PATH=$PATH:$HOME/pkg/bin:$HOME/pkg/sbin:$HOME/pkg/qt4/bin:$HOME/pkg/usr/bin:$HOME/pkg/usr/sbin\n");
    pkgProfile.append("export MANPATH=$MANPATH:$HOME/pkg/man\n");
    pkgProfile.append("if [ -z \"${XDG_DATA_DIRS}\" ]; then\n");
    pkgProfile.append("export XDG_DATA_DIRS=/usr/share:/usr/local/share:$HOME/pkg/share:$HOME/pkg/usr/share\n");
    pkgProfile.append("else\n");
    pkgProfile.append("export XDG_DATA_DIRS=$XDG_DATA_DIRS:$HOME/pkg/share:$HOME/pkg/usr/share\n");
    pkgProfile.append("fi\n");
    pkgProfile.append("export XDG_DATA_DIRS\n");
    pkgProfile.append("# UserPKG Integration [END]\n\n");

    QFile userBashProfile(QDir::homePath()+"/.bash_profile");

    if (userBashProfile.exists())
    {
        QString line;
        bool profileOK = false;
        if (userBashProfile.open(QIODevice::ReadOnly))
        {
            QTextStream textStream(&userBashProfile);
            line = textStream.readAll();
            if (line.contains("UserPKG"))
            {
                profileOK=true;
            }
        }
        userBashProfile.close();

        if (!profileOK)
        {
            if (userBashProfile.open(QIODevice::WriteOnly))
            {
                QTextStream textStream(&userBashProfile);
                textStream << line;
                textStream << pkgProfile;
            }
            userBashProfile.close();
            if (tray->isVisible())
            {
                tray->showMessage("Profile modified","Modified your bash_profile, you must logout to apply modifications later");
            }
        }
    }
    else
    {
        if (userBashProfile.open(QIODevice::WriteOnly))
        {
            QTextStream textStream(&userBashProfile);
            textStream << pkgProfile;
        }
        if (tray->isVisible())
        {
            tray->showMessage("Profile modified","Modified your bash_profile, you must logout to apply modifications later");
        }
    }
    userBashProfile.close();

    /*QDir userpkgDir(QDir::homePath()+"/pkg");
    QDir userpkgDirTmp(QDir::homePath()+"/tmp");
    QDir userpkgDirPkg(QDir::homePath()+"/.pkgdb");
    QDir userpkgDirVar(QDir::homePath()+"/pkg/var");
    QDir userpkgDirpkgsrc(QDir::homePath()+"/pkgsrc");
*/
    /*if (!userpkgDir.exists())
    {
        userpkgDir.mkdir(QDir::homePath()+"/userpkg");
    }
    if (!userpkgDirTmp.exists())
    {
        userpkgDirTmp.mkdir(QDir::homePath()+"/userpkg/tmp");
    }
    if (!userpkgDirPkg.exists())
    {
        userpkgDirPkg.mkdir(QDir::homePath()+"/userpkg/pkg");
    }
    if (!userpkgDirVar.exists())
    {
        userpkgDirVar.mkdir(QDir::homePath()+"/userpkg/var");
    }*/
  /*  if (!userpkgDirpkgsrc.exists())
    {
        qDebug("foo");
        QFile pkgsrcTar(QDir::homePath()+"/tmp/pkgsrc.tar.xz");
        if (pkgsrcTar.exists())
        {
            qDebug("bar");
            if (pkgsrc.extractStart())
            {qDebug("OK");}
        }
        else
        {
            if (pkgsrc.downloadStart())
            {
                ui->statusBar->showMessage("Need to download pkgsrc, please wait ...");
            }
            else
            {
                ui->statusBar->showMessage("Failed to write to temp");
            }
        }
        pkgsrcTar.close();
    }
    else
    {
        if (!pkgsrc.bootstrapStart())
        {
            status = true;
        }
    }*/

    catGen();
    pkgsrc.packagesInstalledRequest();
    pkgsrc.packagesVulnsRequest();

    return status;
}

void MainWindow::pkgsrcDownloadFinished(int status)
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
    if (status==1)
    {
        ui->statusBar->showMessage("pkgsrc downloaded");
        //if (pkgsrc.extractStart())
        //{
        //    ui->statusBar->showMessage("Extracting pkgsrc ...");
        //}
        //else
        //{
        //    ui->statusBar->showMessage("Unable to extract pkgsrc, please check file permissions");
        //}
    }
    else
    {
        ui->statusBar->showMessage("Download of pkgsrc failed");
    }
}

void MainWindow::pkgsrcDownloadStatus(qint64 start, qint64 end)
{
    if(ui->progressBar->isHidden())
    {
        ui->progressBar->show();
    }
    ui->statusBar->showMessage("Downloading pkgsrc ...");
    if (end>0)
    {
        ui->progressBar->setMaximum(end);
    }
    else
    {
        ui->progressBar->setMaximum(29360128); // Hack! Since tnftp returns -1
    }
    ui->progressBar->setValue(start);
}

void MainWindow::bootstrapExtractFinished(int status)
{
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->hide();
    if (status==0)
    {
        ui->statusBar->showMessage("Done extracting pkgsrc");
        //this->setWindowTitle("UserPKG " + pkgsrc.branchVersion());
        //pkgsrc.bootstrapStart();
    }
    else
    {
        ui->statusBar->showMessage("Failed to extract pkgsrc");
    }
}

void MainWindow::bootstrapExtractRead()
{
    ui->statusBar->showMessage("Extracting pkgsrc ...");
}

void MainWindow::bootstrapMakeFinished(int status)
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    if (!ui->progressBar->isHidden())
    {
        ui->progressBar->hide();
    }

    if (status==0)
    {
        if(tray->isVisible())
        {
            tray->showMessage("Bootstrap","Bootstrapping done, you should now logout to apply desktop integration");
        }
        ui->statusBar->showMessage("Bootstrapping done, you should now logout to apply desktop integration");
        pkgsrc.packageCleanRequest();
        catGen();
        installedPackagesGenTree();
    }
    else
    {
        if(tray->isVisible())
        {
            tray->showMessage("Bootstrap","Bootstrap failed, please check log");
        }
        ui->statusBar->showMessage("Bootstrapping failed, please check log");
    }
}

void MainWindow::bootstrapMakeRead(QString data)
{
    if (ui->progressBar->isHidden())
    {
        ui->progressBar->show();
    }
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);

    ui->statusBar->showMessage("Bootstrapping, please wait ...");
    ui->log->appendPlainText(data);
}

void MainWindow::on_queue_customContextMenuRequested()
{
    QMenu *menu=new QMenu;
    int row = 0;
    row = ui->queue->topLevelItemCount();
    QTreeWidgetItem *item = ui->queue->currentItem();
    if (row>0 && item)
    {
        if (item->data(2,3).toInt()==1||item->data(2,3).toInt()>2) {
            QAction* delRow = new QAction("Remove from queue",this);
            connect(delRow,SIGNAL(triggered()),this,SLOT(delPackageFromQueue()));
            menu->addAction(delRow);
        }
        /*if (item->data(2,3).toInt()==4) {
            QAction* retryRow = new QAction("Requeue",this);
            connect(retryRow,SIGNAL(triggered()),this,SLOT(queueRetry()));
            menu->addAction(retryRow);
        }*/
        if (pkgsrc.bmakeActive()) {
            QAction* cancelBuild = new QAction("Cancel Build",this);
            connect(cancelBuild,SIGNAL(triggered()),this,SLOT(queueStop()));
            menu->addAction(cancelBuild);
        }
        else {
            QAction* startBuild = new QAction("Start Build",this);
            connect(startBuild,SIGNAL(triggered()),this,SLOT(queueStart()));
            menu->addAction(startBuild);
        }
    }
    menu->exec(QCursor::pos());
}

void MainWindow::queueRetry()
{ // Broken
    QTreeWidgetItem *item = ui->queue->currentItem();
    if (item)
        item->setData(2,3,1);
}

void MainWindow::addPackageToQueue(QString package, QString category, QString options)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0,package);
    item->setData(0,3,category);
    item->setText(1,options);
    item->setText(2,"Pending");
    item->setData(2,3,1);
    item->setIcon(0,QIcon(":/files/queue-package.png"));
    item->setIcon(2,QIcon(":/files/queue-pending.png"));
    ui->queue->addTopLevelItem(item);
    if (!pkgsrc.bmakeActive())
    {
        queueStart();
    }
}

void MainWindow::delPackageFromQueue()
{
    QTreeWidgetItem *item = ui->queue->currentItem();
    if (item) {
        delete item;
        catGen();
    }
}
void MainWindow::queueStart()
{
    if (!pkgsrc.bmakeActive()) {
        int queueCount=0;
        int queueItems=ui->queue->topLevelItemCount();
        int queueItem=-1;
        if (ui->queue->topLevelItemCount()>0)
        {
            while (queueCount<queueItems) {
                if (ui->queue->topLevelItem(queueCount)->data(2,3).toInt()==1) {
                    queueItem=queueCount;
                    queueItems=queueCount;
                }
                queueCount++;
            }
            if (queueItem>-1)
            {
                if (pkgsrc.bmakeStart(ui->queue->topLevelItem(queueItem)->text(0),ui->queue->topLevelItem(queueItem)->data(0,3).toString(),ui->queue->topLevelItem(queueItem)->text(1),"install"))
                {
                    ui->queue->topLevelItem(queueItem)->setData(2,3,2);
                    ui->queue->topLevelItem(queueItem)->setText(2,"Working ...");
                    ui->queue->topLevelItem(queueItem)->setIcon(2,QIcon(":/files/queue-busy.png"));
                }
            }
        }
    }
}

void MainWindow::queueStop()
{
    if (pkgsrc.bmakeStop())
        ui->queue->clear();
    //queue->close();
    /*ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    if (!ui->progressBar->isHidden())
        ui->progressBar->hide();
    pkgCleanExec();*/
}

void MainWindow::queueFinished(int status)
{
    if (status==0)
        ui->statusBar->showMessage("Build done");
    else
        ui->statusBar->showMessage("Build failed");


    //if (status>=0)
      //  queue->close();


    //if (status ==0)
    //{
        int queueCount=0;

        QString activePkg;
        if (ui->queue->topLevelItemCount()>=0)
        {
            while (queueCount<ui->queue->topLevelItemCount()) {
                if (ui->queue->topLevelItem(queueCount)->data(2,3).toInt()==2) {
                    activePkg= ui->queue->topLevelItem(queueCount)->text(0);
                    if (status==0) {
                        ui->queue->topLevelItem(queueCount)->setData(2,3,3);
                        ui->queue->topLevelItem(queueCount)->setIcon(2,QIcon(":/files/queue-done.png"));
                        ui->queue->topLevelItem(queueCount)->setText(2,"Finished");
                        if (tray->isVisible())
                            tray->showMessage(ui->queue->topLevelItem(queueCount)->text(0),"Build finished");
                    }
                    else {
                        ui->queue->topLevelItem(queueCount)->setData(2,3,4);
                        ui->queue->topLevelItem(queueCount)->setIcon(2,QIcon(":/files/queue-error.png"));
                        ui->queue->topLevelItem(queueCount)->setText(2,"Failed");
                        if (tray->isVisible())
                            tray->showMessage(ui->queue->topLevelItem(queueCount)->text(0),"Build failed");
                    }
                }
                queueCount++;
            }
        }

        /*int queuePending = 0;
        int queuePendingCount=0;
        while (queuePendingCount<ui->queue->topLevelItemCount()) {
            if (ui->queue->topLevelItem(queuePendingCount)->data(2,3).toInt()==1)
                queuePending++;
            queuePendingCount++;
        }*/

        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
        if (!ui->progressBar->isHidden())
            ui->progressBar->hide();
        installedPackagesGenTree();
        //pkgsrcPkgVulnCheckExec();
        pkgsrc.packagesVulnsRequest();

        // Make pretty log for debugging
        if (status !=0) {
            ui->failedLog->append("<p style=\"color:red;text-transform:uppercase;\">********** FAILED LOG FOR "+activePkg+" START **********</p>");
            QStringList logLine=ui->log->toPlainText().split("\n",QString::SkipEmptyParts);
            for (int i = 0; i < logLine.size(); ++i)
            {
                QString line;
                if (logLine.at(i).contains("=>")) {
                    if (logLine.at(i).contains("Installing")||logLine.at(i).contains("Install binary")||logLine.at(i).contains("OK")||logLine.at(i).contains("Build")||logLine.at(i).contains("Config"))
                        line.append("<div style=\"color:lightgreen;\">");
                    else
                        line.append("<div style=\"color:gray;\">");
                }
                if (logLine.at(i).contains("pkg_add")||logLine.at(i).contains("WARNING:")||logLine.at(i).contains("bmake:")||logLine.at(i).contains("exit status")||logLine.at(i).contains("warning:")) {
                    if (logLine.at(i).contains("failed"))
                        line.append("<div style=\"color:red;\">");
                    else
                        line.append("<div style=\"color:yellow;\">");
                }
                if (logLine.at(i).contains("*** Error")||logLine.at(i).contains("ERROR:")||logLine.at(i).contains("]Error")||logLine.at(i).contains("undefined reference")||logLine.at(i).contains("error:"))
                    line.append("<div style=\"color:red;\">");

                if (line.isEmpty())
                    line.append(logLine.at(i));
                else {
                    line.append(logLine.at(i));
                    line.append("</div>");
                }
                if (!line.isEmpty())
                    ui->failedLog->append(line);
            }
            ui->failedLog->append("<p style=\"color:gray;text-transform:uppercase;\">********** FAILED LOG FOR "+activePkg+" END **********</p>");
        }
        //pkgCleanExec();
        //pkgsrc.packageCleanRequest();
        pkgsrc.packagesInstalledRequest();


        ui->log->clear();
        //ui->statusBar->showMessage("Done", 500);
        int queuePending = 0;
        int queuePendingCount=0;
        while (queuePendingCount<ui->queue->topLevelItemCount())
        {
            if (ui->queue->topLevelItem(queuePendingCount)->data(2,3).toInt()==1)
            {
                queuePending++;
            }
            queuePendingCount++;
        }
        if (queuePending>0)
        {
            queueStart();
        }


        //ui->statusBar->showMessage("Cleaning build ...");

        //pkgClean->waitForFinished(-1);
        /*if (queuePending>0)
            queueStart();*/
}

void MainWindow::queueRead(QString data)
{
/*    if(ui->progressBar->isHidden())
        ui->progressBar->show();
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
*/
    //ui->statusBar->showMessage("Building ...");
    QString line = data;
    if (line.contains("=>")) {
        QString status = line;
        //ui->statusBar->showMessage(status.replace("=","").replace(">",""));
        if (line.contains("Building for")||line.contains("Configuring for")||line.contains("Installing for")||line.contains("Returning to build")||line.contains("Extracting for")||line.contains("Fetching "))
        {
            int queueCount=0;
            //QString activePkg;
            if (ui->queue->topLevelItemCount()>=0)
            {
                //qDebug("ok");
                while (queueCount<ui->queue->topLevelItemCount()) {
                    if (ui->queue->topLevelItem(queueCount)->data(2,3).toInt()==2) {
                        //activePkg= ui->queue->topLevelItem(queueCount)->text(0);
                        //if (status==0) {
                            //ui->queue->topLevelItem(queueCount)->setData(2,3,3);
                            //ui->queue->topLevelItem(queueCount)->setIcon(2,QIcon(":/files/queue-done.png"));
                        ui->queue->topLevelItem(queueCount)->setText(2,status.replace("=","").replace("> ","").replace("\n",""));
                        //ui->statusBar->showMessage(status.replace("=","").replace("> ","").replace("\n",""));
                        ui->statusBar->showMessage("Queue is running ...");
                            //if (tray->isVisible())
                              //  tray->showMessage(ui->queue->topLevelItem(queueCount)->text(0),"Build finished");
                        //}
                        /*else {
                            ui->queue->topLevelItem(queueCount)->setData(2,3,4);
                            ui->queue->topLevelItem(queueCount)->setIcon(2,QIcon(":/files/queue-error.png"));
                            ui->queue->topLevelItem(queueCount)->setText(2,"Failed");
                            if (tray->isVisible())
                                tray->showMessage(ui->queue->topLevelItem(queueCount)->text(0),"Build failed");
                        }*/
                    }
                    queueCount++;
                }
            }
        }
    }
    //else {ui->statusBar->showMessage("Queue is running ...");}
   // ui->statusBar->showMessage("Queue is active");

    ui->log->appendPlainText(line);
}


/*void MainWindow::pkgCleanFinished(int status)
{
    //pkgClean->close();

    //if (!pkgsrc.bmakeActive())
    //{
        if (!ui->progressBar->isHidden())
        {
            ui->progressBar->hide();
        }
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
    //}


    if (status==0)
    {
        ui->log->clear();
        ui->statusBar->showMessage("Done", 500);
        int queuePending = 0;
        int queuePendingCount=0;
        while (queuePendingCount<ui->queue->topLevelItemCount())
        {
            if (ui->queue->topLevelItem(queuePendingCount)->data(2,3).toInt()==1)
            {
                queuePending++;
            }
            queuePendingCount++;
        }
        if (queuePending>0)
        {
            queueStart();
        }
    }
    else
    {
        ui->statusBar->showMessage("Clean failed", 1000);
    }
}*/


void MainWindow::pkgsrcPkgVersionFinished(QString version)
{
    ui->pkg->setDisabled(false);
    ui->cat->setDisabled(false);
    if (!pkgsrc.bmakeActive()) {
        if (!ui->progressBar->isHidden())
            ui->progressBar->hide();
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
        ui->statusBar->showMessage("Done",1000);
    }
    //QString version;
    //version = pkgsrcPkgVersion->readAll();
    //pkgsrcPkgVersion->close();
    if (!version.isEmpty())
        ui->pkgVersion->setText(version);
    else
        ui->pkgVersion->clear();
}

void MainWindow::pkgsrcPkgNameFinished(QString name)
{
    ui->pkg->setDisabled(false);
    ui->cat->setDisabled(false);
    if (!pkgsrc.bmakeActive()) {
        if (!ui->progressBar->isHidden())
            ui->progressBar->hide();
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
        ui->statusBar->showMessage("Done",1000);
    }
    //QString name;
    //name = pkgsrcPkgName->readAll();
    //pkgsrcPkgName->close();
    if (!name.isEmpty())
        ui->pkgName->setText(name);
    else
        ui->pkgName->clear();
}

bool MainWindow::pkgsrcPkgInstalled(QString name)
{
    int status = false;

    int installedCount=0;
    int installedItems=ui->installedPackagesTree->topLevelItemCount();

    if (ui->installedPackagesTree->topLevelItemCount()>0)
    {
        while (installedCount<installedItems) {
                QString installed = ui->installedPackagesTree->topLevelItem(installedCount)->text(0);
                if (name == installed)
                    status = true;
            installedCount++;
        }
    }

    int queueCount=0;
    int queueItems=ui->queue->topLevelItemCount();

    if (ui->queue->topLevelItemCount()>0)
    {
        while (queueCount<queueItems) {
                QString queued = ui->queue->topLevelItem(queueCount)->text(0);
                if (name == queued)
                    status = true;
            queueCount++;
        }
    }

    return status;
}

void MainWindow::pkgsrcPkgVulnCheckFinished(QStringList result)
{
    if (!result.isEmpty())
    {
        ui->pkgVuln->clear();
        for (int i = 0; i < result.size(); ++i)
        {
            QStringList vulnList = result.at(i).split("|",QString::SkipEmptyParts);
            QString itemInfo;
            QString itemUrl;
            if (!vulnList.isEmpty())
            {
                itemInfo = vulnList.takeFirst();
                if (!vulnList.isEmpty())
                {
                    itemUrl = vulnList.takeLast();
                }
            }
            if (!itemInfo.isEmpty()&&!itemUrl.isEmpty())
            {
                QTreeWidgetItem *item = new QTreeWidgetItem;
                item->setText(0,itemInfo);
                item->setData(0,3,itemUrl);
                item->setIcon(0,QIcon(":/files/queue-package.png"));
                ui->pkgVuln->addTopLevelItem(item);
            }
        }
    }
}

void MainWindow::on_installedPackagesTree_customContextMenuRequested()
{
    int row = 0;
    row = ui->installedPackagesTree->topLevelItemCount();
    QTreeWidgetItem *item = ui->installedPackagesTree->currentItem();
    if (row>0 && item&&!pkgsrc.bmakeActive()&&!pkgsrc.bootstrapActive())
    {
            QMenu *menu=new QMenu;
            QAction* delPkg = new QAction("Uninstall package",this);
            connect(delPkg,SIGNAL(triggered()),this,SLOT(delPackage()));
            menu->addAction(delPkg);
            menu->exec(QCursor::pos());
    }

}

void MainWindow::delPackage()
{
    QTreeWidgetItem *item = ui->installedPackagesTree->currentItem();
    if (item) {

    if (!item->text(0).isEmpty()&&!pkgsrc.bmakeActive()&&!pkgsrc.bootstrapActive())
    {
        if (pkgsrc.packageRemove(item->text(0),0))
            ui->statusBar->showMessage("Removing package, please wait ...");
    }
    else
    {
        ui->statusBar->showMessage("no package selected, or queue is running");
    }

    }
}

void MainWindow::delPackageDone(int result)
{
    if (result==0) {
        ui->statusBar->showMessage("Package removed", 1000);
        //installedPackagesGenTree();
        pkgsrc.packagesInstalledRequest();
        catGen();
    }
    //else
        //ui->statusBar->showMessage("Failed to remove package",5000);
}

void MainWindow::delPackageLog(QString text)
{
    QString line = text;
    ui->log->appendPlainText(line);
    qDebug() << line;
    if (line.contains("required by other packages")||line.contains("not for deletion")) {
        ui->statusBar->showMessage(line.replace("pkg_delete:",""),10000);
    }
}

void MainWindow::on_actionSync_triggered()
{
    ui->statusBar->showMessage("Syncing pkgsrc, please wait ...");
    pkgsrc.updatePkgsrc();
    connect(&pkgsrc,SIGNAL(pkgsrcUpdateFinished(int)),this,SLOT(pkgsrcSyncDone(int)));
    connect(&pkgsrc,SIGNAL(pkgsrcUpdateStatus(QString)),this,SLOT(pkgsrcSyncLog(QString)));
}

void MainWindow::pkgsrcSyncLog(QString log)
{
    ui->log->appendPlainText(log);
    if (!log.isEmpty()) {
        if(!log.contains("ignored"))
            ui->statusBar->showMessage("pkgsrc sync status: "+log.replace("cvs update: Updating",""));
    }
}

void MainWindow::pkgsrcSyncDone(int status)
{
    qDebug()<<"foo"<<status;
    if (status==0)
        ui->statusBar->showMessage("Sync complete",5000);
    else
        ui->statusBar->showMessage("Sync failed",50000);
}
