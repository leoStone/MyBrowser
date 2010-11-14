#include "update.h"

update_vers::update_vers(QObject *parent) : QObject(parent)
{
    pfad = QCoreApplication::applicationDirPath();
    upgedated = false;

}
update_vers::~update_vers()
{
    if(upgedated == true)
        delete file;
    delete b_array;
}
void update_vers::run_request_for_for_update()
{
    QUrl request_1_url;
    request_1_url.setUrl(QString("http://mybrows.co.de/update/update.php?vers=342.021,33"));
    //Nachdem nun die Url bestimmt wurde, die Abfrage ausführen
    //
    qnam = new QNetworkAccessManager(this);
    connect(qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(read_request_1_answer_action(QNetworkReply *)));
    qnam->get(QNetworkRequest(request_1_url));

}
void update_vers::read_request_1_answer_action(QNetworkReply *reply)
{
    b_array = new QByteArray(reply->readAll());
    if(b_array->startsWith('t') == true)
    {
            qnam_2 = new QNetworkAccessManager(this);
            connect(qnam_2, SIGNAL(finished(QNetworkReply*)), this, SLOT(read_request_2_answer(QNetworkReply *)));
            qnam_2->get(QNetworkRequest(QUrl(QString("http://www.mybrows.co.de/update/update.exe"))));
            return;

    }
}
void update_vers::read_request_2_answer(QNetworkReply *reply_2)
{
    pfad.append("\\update.exe");
    file = new QFile(pfad);
    file->open(QIODevice::WriteOnly);
    file->write(reply_2->readAll());
    file->flush();
    file->close();
    upgedated = true;
}
