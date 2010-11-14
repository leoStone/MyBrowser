#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "includer.h"

namespace Ui {
    class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
    QSize size;
    QPoint point;
    bool en_max;
    QString lastUrl;
    bool en_sav_set;
    bool en_cookies;
    bool en_plugins;
    bool en_tor;
    bool en_homePage;
    bool en_treeBar;
    QString path_tor_inst;
    QString path_cur_home;
    Ui::settings *ui;
signals:
    void requestForValues();
    void settingsChagned();
    void requestForCurrentPage();
public slots:
    void but_info_clicked();
    void but_tor_search_clicked();
    void but_qt_infoclicked();
    void save();
    void set();
    void startRequestForHomePage();
};

#endif // SETTINGS_H
