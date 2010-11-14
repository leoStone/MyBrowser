#include "downloadmanager.h"
#include "ui_downloadmanager.h"

DownloadManager::DownloadManager(QWidget *parent, QNetworkAccessManager *pqnam) :
    QDialog(parent),
    ui(new Ui::DownloadManager), qnam(pqnam)
{
    ui->setupUi(this);
    if(isVisible())
        hide();
    connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->butOpen, SIGNAL(clicked()), this, SLOT(open()));
    downloadInProgress = false;
}

DownloadManager::~DownloadManager()
{
    delete ui;
}
void DownloadManager::Download(QNetworkRequest request)
{
    QFileInfo info(request.url().path());
    QString fileName = info.fileName();
    if(QFile::exists(fileName)){
        int i = NULL;
    do{
        fileName.insert(0, tr("(%1)").arg(i));
        i++;
    }while(QFile::exists(fileName));}
    fileName.insert(0, QCoreApplication::applicationDirPath() + tr("//Downloads//"));
    ui->labelZiel->setText(fileName);
    ui->labelUrl->setText(request.url().toString());
    file = new QFile(fileName);
    if(!file->open(QFile::WriteOnly)){
        QMessageBox::warning(this, tr("ERROR"), tr("Konnte die Datei nicht zum Schreiben Öffnen!\n (ErrNo. 1"), QMessageBox::Ok);
        return;}
    ui->butCancel->setEnabled(true);
    ui->butOpen->setEnabled(false);
    ui->chOpenAfter->setEnabled(true);
    reply = qnam->get(request);
    downloadInProgress = true;
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
    if(!isVisible())
        show();

}
void DownloadManager::progress(qint64 read, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(read);
    ui->labelTotal->setText(tr("%1").arg(total));
    ui->labelLoaded->setText(tr("%1").arg(read));
}
void DownloadManager::readyToRead()
{
    file->write(reply->readAll());
}
void DownloadManager::finished()
{
    file->flush();
    file->close();
    if(ui->chOpenAfter->isChecked())
        open();
    ui->butCancel->setDisabled(true);
    ui->butOpen->setEnabled(true);

    reply->deleteLater();
    reply = 0;

}
void DownloadManager::cancel()
{
    reply->abort();
    file->close();
    file->remove();
    reply->deleteLater();
    reply = 0;
    if(isVisible())
        hide();
}
void DownloadManager::open()
{
    QUrl buffer = QUrl::fromLocalFile(ui->labelZiel->text());
    QDesktopServices::openUrl(buffer);
    if(isVisible())
        hide();
}
void DownloadManager::downloadUnsupportedContent(QNetworkReply *preply)
{
    reply = preply;
    QFileInfo info(reply->url().path());
    QString fileName = info.fileName();
    if(QFile::exists(fileName)){
        int i = NULL;
    do{
        fileName.insert(0, tr("(%1)").arg(i));
        i++;
    }while(QFile::exists(fileName));}
    fileName.insert(0, QCoreApplication::applicationDirPath() + tr("//Downloads//"));
    ui->labelZiel->setText(fileName);
    ui->labelUrl->setText(request.url().toString());
    file = new QFile(fileName);
    if(!file->open(QFile::WriteOnly)){
        QMessageBox::warning(this, tr("ERROR"), tr("Konnte die Datei nicht zum Schreiben Öffnen!\n (ErrNo. 1"), QMessageBox::Ok);
        return;}
    ui->butCancel->setEnabled(true);
    ui->butOpen->setEnabled(false);
    ui->chOpenAfter->setEnabled(true);
    downloadInProgress = true;
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
    if(!isVisible())
        show();
}
