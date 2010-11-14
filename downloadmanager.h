#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QDialog>
#include "includer.h"

namespace Ui {
    class DownloadManager;
}

class DownloadManager : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadManager(QWidget *parent, QNetworkAccessManager *pqnam);
    ~DownloadManager();
    bool downloadInProgress;
public slots:
    void Download(QNetworkRequest rquest);
    void finished();
    void cancel();
    void open();
    void progress(qint64 read, qint64 total);
    void readyToRead();
    void downloadUnsupportedContent(QNetworkReply *preply);

private:
    Ui::DownloadManager *ui;
    QFile *file;
    QNetworkReply *reply;
    QNetworkAccessManager *qnam;
};

#endif // DOWNLOADMANAGER_H
