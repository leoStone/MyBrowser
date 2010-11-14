#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_authentification.h"
#include "ui_settings.h"

MainWindow::MainWindow(QString file_adress, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    process = new QProcess(this);

    pfad = QCoreApplication::applicationDirPath();

    QString buffer = QCoreApplication::applicationDirPath();
    buffer.append("\\update.exe");

    if(QFile::exists(buffer) == true)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(buffer));

        process->terminate();

    }

    QSplashScreen *splash = new QSplashScreen(QPixmap(":/tbicons/img/logo.png"), Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("Laden...", Qt::AlignBottom | Qt::AlignCenter);

    ui->setupUi(this);
    this->hide();

    set = new settings(this);
    icon = new favicon(this);
    search_result = new gsuggests(ui->lineEdit);
    versionspruefung = new update_vers(this);
    versionspruefung->run_request_for_for_update();
    cookie = new cookiejar(this, set->en_cookies);
    bok = new bookmarks(this, ui->tree_bookmarks, ui->but_hinzu, ui->but_edit, ui->but_addFolder);
    download = new DownloadManager(this, qnam);

    m_faviconTimer = new QTimer(this);
    m_editorTimer = new QTimer(this);


    qnam = new QNetworkAccessManager(this);
    page = new QWebPage(this);

    ui->webView->setPage(page);
    ui->webView->page()->setNetworkAccessManager(qnam);
    QWebSettings::setIconDatabasePath(pfad);
    QWebSettings::setOfflineStoragePath((pfad + tr("\\offline_cache")));

    QString cache_path = pfad;
    cache_path.append("\\cache");
    cache = new QNetworkDiskCache(qnam);
    cache->setCacheDirectory(cache_path);
    qnam->setCache(cache);

    qnam->setCookieJar(cookie);


    // Bei klick auf Hme Button oder F6 funktion home aufrufen
    connect(ui->home_button, SIGNAL(clicked()), this, SLOT(home()));
    //Bei einem ändern der URl die neu_address auf rufen
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(neu_adresse(QUrl)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(neu_adresse_ein()));

    connect(ui->webView, SIGNAL(loadStarted()), this, SLOT(load_in_progress()));

    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(load_finished(bool)));

    connect(ui->reload_button, SIGNAL(clicked()), this, SLOT(reload_button()));

    connect(ui->webView, SIGNAL(iconChanged()), this, SLOT(icon_changed()));

    connect(ui->webView, SIGNAL(titleChanged(QString)), this, SLOT(titelChanged(QString)));

    connect(ui->but_book, SIGNAL(clicked()), this, SLOT(showBar()));

    connect(ui->webView, SIGNAL(loadProgress(int)), ui->barLoadProgress, SLOT(setValue(int)));

    connect(ui->sliderrSiteZoom, SIGNAL(valueChanged(int)), this, SLOT(sliderSliding(int)));
    connect(ui->butSetZoomToNorm, SIGNAL(clicked()), this, SLOT(sliderSliding()));

    //für bookmarks
    connect(ui->tree_bookmarks, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openBookmark(QTreeWidgetItem*, int)));

    //connections für favicons
    connect(icon, SIGNAL(neuer_icon()), this, SLOT(neuer_icon()));
    connect(icon, SIGNAL(standart_icon()), this, SLOT(set_standart_icon()));
    connect(m_faviconTimer, SIGNAL(timeout()), this, SLOT(lookForNewFavicon()));

    //connections für settings
    connect(ui->but_settings, SIGNAL(clicked()), set, SLOT(show()));
    connect(set, SIGNAL(requestForValues()), this, SLOT(returnRequestedValues()));
    connect(set, SIGNAL(settingsChagned()), this, SLOT(settingsChanged()));
    connect(set, SIGNAL(requestForCurrentPage()), this, SLOT(requestForCurrentPage()));
    connect(set->ui->but_cl_cache, SIGNAL(clicked()), this, SLOT(requestForCacheClearing()));
    //für qnam
    connect(qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authentification_requied(QNetworkReply*,QAuthenticator*)));

    //für downloads
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), download, SLOT(Download(QNetworkRequest)));
    connect(ui->butDownloadManager, SIGNAL(clicked()), download, SLOT(show()));

    ui->lineEdit->installEventFilter(this);

    if(file_adress != NULL)
        ui->webView->load(QUrl(file_adress));
    else
    {
        if(!set->en_homePage)
            ui->webView->load(QUrl(set->lastUrl));
        else
            ui->webView->load(QUrl(set->path_cur_home));
    }

    this->resize(set->size);
    this->move(set->point);
    if(set->en_max)
        this->showMaximized();

    if(!set->en_treeBar)
        ui->tabWidget->hide();


    splash->finish(this);
    this->show();

    m_faviconTimer->start(3000);
    shutted = true;


}

MainWindow::~MainWindow()
{
    if(download->downloadInProgress){
        if(QMessageBox::warning(this, tr("Download noch nicht Beendet!"), tr("Ein Download ist Grade noch in Bearbeitung wirklich Beenden?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes){
            download->cancel();
        }
    else
    return;
    }
    set->point = this->pos();
    set->size = this->size();
    set->lastUrl = ui->webView->url().toString();
    set->en_treeBar = ui->tabWidget->isVisible();
    delete bok;
    delete cookie;
    delete set;
    delete icon;
    delete versionspruefung;
    delete process;
    delete search_result;
    delete ui;
}
void MainWindow::home()
{
    ui->webView->load(set->path_cur_home);
}
void MainWindow::neu_adresse(QUrl url)
{
    ui->lineEdit->setText(url.toString());
    bok->curAdress = ui->webView->url().toString();
    bok->curTitel = ui->webView->title();
    ui->back_button->setFlat(!ui->webView->page()->history()->canGoBack());
    ui->forward_button->setFlat(!ui->webView->page()->history()->canGoForward());
    m_faviconTimer->start(3000);

}
void MainWindow::neu_adresse_ein()
{
    QString string(ui->lineEdit->text());
    ui->webView->setFocus();
    ui->webView->load(QUrl(QUrl::fromUserInput(string)));

}
void MainWindow::load_in_progress()
{
    ui->reload_button->setIcon(QIcon(":/tbicons/img/cross.png"));
    progress = true;
    ui->stat->setText(tr("Lade..."));

}
void MainWindow::load_finished(bool finished)
{
    if(finished == true)
    {
        progress = false;
        icon_changed();
        ui->stat->setText(tr("Bereit."));
    }

}
bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj != ui->lineEdit)
        return false;
    if(ev->type() == QEvent::MouseButtonPress && shutted == true)
    {
        ui->lineEdit->selectAll();
        m_editorTimer->singleShot(1000, this, SLOT(EditorTimerShut()));
        shutted = false;
        return true;
    }
    else
        return false;
}
void MainWindow::neuer_icon()
{
    QString icon_path = pfad;
    icon_path.append("\\favicon.ico");
    ui->reload_button->setIcon(QIcon(icon_path));
}
void MainWindow::set_standart_icon()
{
    ui->reload_button->setIcon(QIcon(":/tbicons/img/world.png"));
}
void MainWindow::reload_button()
{
    if(progress == true)
       { ui->webView->stop();}
    else
        ui->webView->reload();
}
void MainWindow::authentification_requied(QNetworkReply *r, QAuthenticator *a)
{
    QDialog pwdialog(this);
    Ui::authentification pwdui;
    pwdui.setupUi(&pwdialog);
    if(pwdialog.exec() == QDialog::Accepted)
    {
        a->setUser(pwdui.lineEdit->text());
        a->setPassword(pwdui.lineEdit_2->text());
    }
}
void MainWindow::icon_changed()
{
    QIcon buffer = QWebSettings::iconForUrl(ui->webView->url());
    if(buffer.isNull() == false)
        ui->reload_button->setIcon(buffer);
    else
        icon->neue_url(ui->webView->url());
}
void MainWindow::returnRequestedValues()
{
    set->point = this->pos();
    set->size = this->size();
    set->en_max = this->isMaximized();
    set->lastUrl = ui->webView->url().toString();
    set->save();
}
void MainWindow::settingsChanged()
{
    cookie->set_m_saveCookies(set->en_cookies);
    if(set->en_tor){
        QNetworkProxy tor;
        tor.setHostName(tr("localhost"));
        tor.setPort(8118);
        tor.setType(QNetworkProxy::HttpProxy);
        qnam->setProxy(tor);
    }
    else
        qnam->setProxy(QNetworkProxy());
}
void MainWindow::titelChanged(QString newTitle)
{
    this->setWindowTitle(newTitle + tr(" - MyBrowser"));
}
void MainWindow::requestForCurrentPage()
{
    set->ui->line_path_home_seite->setText(ui->webView->url().toString());
}
void MainWindow::requestForCacheClearing()
{
    if(QMessageBox::warning(this, tr("Wirklich leeren?"), tr("Wirklich den Kompletten Cache leeren?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        qnam->cache()->clear();

}
void MainWindow::openBookmark(QTreeWidgetItem *item, int i)
{
    if(item->data(0, Qt::UserRole) == "folder")
        return;
    ui->webView->load(QUrl(item->text(1)));
}
void MainWindow::showBar()
{
    if(ui->tabWidget->isHidden())
    {
        ui->tabWidget->show();
        ui->but_book->setDown(true);
    }
    else
    {
        ui->tabWidget->hide();
        ui->but_book->setDown(false);
    }
}
void MainWindow::lookForNewFavicon()
{
    QIcon buffer = QWebSettings::iconForUrl(ui->webView->url());
    if(buffer.isNull())
        icon->neue_url(ui->webView->url());
    else
    {
        ui->reload_button->setIcon(buffer);
        m_faviconTimer->stop();
    }
}
void MainWindow::sliderSliding(int newValue)
{
    qreal buffer = (newValue/100);
    ui->webView->setZoomFactor(buffer);
}
void MainWindow::sliderSliding()
{
    ui->webView->setZoomFactor(1);
    ui->sliderrSiteZoom->setValue(100);
}
void MainWindow::EditorTimerShut()
{
    shutted = true;
}
