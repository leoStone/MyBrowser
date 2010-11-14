#ifndef UPDATE_H
#define UPDATE_H

#include "includer.h"

class update_vers : public QObject
{
    Q_OBJECT
public:
    update_vers(QObject *parent);
    ~update_vers();
    void run_request_for_for_update();
public slots:
    void read_request_1_answer_action(QNetworkReply *reply);
    void read_request_2_answer(QNetworkReply *reply_2);

signals:
    void update_in_progress();
    void update_finished();

private:
    QNetworkAccessManager *qnam;
    QNetworkAccessManager *qnam_2;
    QByteArray *b_array;
    QFile *file;
    QString pfad;
    bool upgedated;

};

#endif // UPDATE_H
