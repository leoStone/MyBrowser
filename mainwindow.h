#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "includer.h"

#include <process.h>

#include "gsuggests.h"
#include "update.h"
#include "favicon.h"
#include "settings.h"
#include "cookiejar.h"
#include "bookmarks.h"
#include "downloadmanager.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QString open_adress = 0, QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    QNetworkAccessManager *qnam;
    bool eventFilter(QObject *obj, QEvent *ev);


public slots:
    void home();
    void neu_adresse(QUrl url);
    void neu_adresse_ein();
    void load_in_progress();
    void load_finished(bool finished);
    void neuer_icon();
    void set_standart_icon();
    void reload_button();
    void authentification_requied(QNetworkReply *r, QAuthenticator *a);
    void icon_changed();
    void returnRequestedValues();
    void settingsChanged();
    void requestForCurrentPage();
    void requestForCacheClearing();
    void titelChanged(QString newTitle);
    void openBookmark(QTreeWidgetItem *item, int i);
    void showBar();
    void lookForNewFavicon();
    void sliderSliding(int newValue);
    void sliderSliding();
    void EditorTimerShut();


private:
    gsuggests *search_result;
    update_vers *versionspruefung;
    favicon *icon;
    settings *set;
    cookiejar *cookie;
    bookmarks *bok;
    DownloadManager *download;
    QString pfad;
    QProcess *process;
    QNetworkDiskCache *cache;
    QWebPage *page;

    bool progress;
    bool shutted;
    QTimer *m_faviconTimer;
    QTimer *m_editorTimer;

};

#endif // MAINWINDOW_H
