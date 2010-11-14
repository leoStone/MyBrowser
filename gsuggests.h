#ifndef GSUGGESTS_H
#define GSUGGESTS_H

#include "includer.h"

class gsuggests : public QObject
{
    Q_OBJECT
public:
    gsuggests(QLineEdit *parent = 0);
    ~gsuggests();
    bool eventFilter(QObject *obj, QEvent *ev);
    void showCompletion(const QStringList &choices, const QStringList &hits);


public slots:
    void doneCompletition();
    void preventSuggest();
    void autoSuggest();
    void handleNetworkData(QNetworkReply *networkreply);
private:
    QLineEdit *editor;
    QTreeWidget *popup;
    QTimer *timer;
    QNetworkAccessManager networkManager;

};

#endif // GSUGGESTS_H
