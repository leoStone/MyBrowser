#include "favicon.h"

favicon::favicon(QObject *parent) :
    QObject(parent)
{
    pfad.clear();
    pfad.append(QCoreApplication::applicationDirPath());
    pfad.append("\\favicon.ico");
    icon = new QFile(pfad);
    qnam = new QNetworkAccessManager();

}
favicon::~favicon()
{
    delete icon;
    delete qnam;
}

void favicon::neue_url(QUrl nue_url)
{
    icon = new QFile(pfad);
    icon->open(QIODevice::WriteOnly);
    int counter = NULL;
    QString root_url = NULL;
    QString url = nue_url.toString();
    QByteArray url_zerlegen = url.toLatin1();
    int i = 0;
    char current = NULL;
    do
    {
        current = url_zerlegen.at(i);
        root_url.append(current);

        if(current == '/')
            counter++;
        i++;

    }while(counter != 3);

    root_url.append("favicon.ico");
    QUrl buffer;
    buffer.setUrl(root_url);
    if(!QFile::exists(buffer.toString()))
    {
        emit standart_icon();
        return;
    }

    qnam = new QNetworkAccessManager();
    qnam->get(QNetworkRequest(QUrl(root_url)));
    connect(qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(fertig_geladen(QNetworkReply*)));
}
void favicon::fertig_geladen(QNetworkReply *reply)
{
    icon->write(reply->readAll());
    icon->flush();
    icon->close();
    emit neuer_icon();
}
