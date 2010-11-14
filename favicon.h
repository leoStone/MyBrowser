#ifndef FAVICON_H
#define FAVICON_H

#include "includer.h"

class favicon : public QObject
{
    Q_OBJECT
public:
    favicon(QObject *parent = 0);
    ~favicon();
    void neue_url(QUrl nue_url);

public slots:
    void fertig_geladen(QNetworkReply *reply);
signals:
    void neuer_icon();
    void standart_icon();
private:
    QNetworkAccessManager *qnam;
    QFile *icon;
    QString pfad;

};

#endif // FAVICON_H
