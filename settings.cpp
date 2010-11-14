#include "settings.h"
#include "ui_settings.h"
#include "ui_info_dialog.h"

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    this->hide();

    QSettings laden(QCoreApplication::applicationDirPath() + tr("//settings.ini"), QSettings::IniFormat);
    en_sav_set = laden.value("en_sav_set", true).toBool();
    en_cookies = laden.value("en_cookies", true).toBool();
    en_plugins = laden.value("en_plugins", true).toBool();
    en_treeBar = laden.value("en_treeBar", false).toBool();
    en_tor     = laden.value("en_tor", false).toBool();
    en_homePage = laden.value("en_homePage", false).toBool();
    path_tor_inst = laden.value("path_tor_inst", tr("C://Programme//Vidalia Bundle//Vidalia//vidalia.exe")).toString();
    path_cur_home = laden.value("path_cur_home", tr("http://heise.de")).toString();
    size = laden.value("size", QSize(600,800)).toSize();
    point = laden.value("point", QPoint(100,100)).toPoint();
    en_max = laden.value("en_max", true).toBool();
    lastUrl = laden.value("lastUrl", tr("http://heise.de")).toString();

    connect(ui->but_info, SIGNAL(clicked()), this, SLOT(but_info_clicked()));
    connect(ui->but_search_tor_inst, SIGNAL(clicked()), this, SLOT(but_tor_search_clicked()));
    connect(ui->but_qt_info, SIGNAL(clicked()), this, SLOT(but_qt_infoclicked()));
    connect(ui->but_set_cur, SIGNAL(clicked()), this, SLOT(startRequestForHomePage()));
    connect(this, SIGNAL(accepted()), this, SLOT(set()));

    ui->line_path_home_seite->setText(path_cur_home);
    ui->line_path_to_tor_inst->setText(path_tor_inst);
    ui->ch_en_cookies->setChecked(en_cookies);
    ui->ch_en_plugins->setChecked(en_plugins);
    ui->ch_atStartup_load_home->setChecked(en_homePage);
    ui->gp_en_tor->setChecked(en_tor);
    ui->gp_settings_save->setChecked(en_sav_set);

    QWebSettings::globalSettings()->setWebGraphic(QWebSettings::MissingImageGraphic, QPixmap(tr(":/tbicons/img/picture_not_found.PNG")));
    QWebSettings::globalSettings()->setWebGraphic(QWebSettings::MissingPluginGraphic, QPixmap(tr(":/tbicons/img/plugin_not_found.PNG")));
    QWebSettings::globalSettings()->setWebGraphic(QWebSettings::DefaultFrameIconGraphic, QPixmap(tr(":/tbicons/img/world.png")));
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
}

settings::~settings()
{
    en_cookies = ui->ch_en_cookies->isChecked();
    en_plugins = ui->ch_en_plugins->isChecked();
    en_tor = ui->gp_en_tor->isChecked();
    en_sav_set = ui->gp_settings_save->isChecked();
    en_homePage = ui->ch_atStartup_load_home->isChecked();
    path_cur_home = ui->line_path_home_seite->text();
    path_tor_inst = ui->line_path_to_tor_inst->text();
    QSettings speichern(QCoreApplication::applicationDirPath() + tr("//settings.ini"), QSettings::IniFormat);
    speichern.setValue("en_sav_set", en_sav_set);
    if(!en_sav_set)
        return;
    speichern.setValue("en_cookies", en_cookies);
    speichern.setValue("en_plugins", en_plugins);
    speichern.setValue("en_tor", en_tor);
    speichern.setValue("en_homePage", en_homePage);
    speichern.setValue("en_treeBar", en_treeBar);
    speichern.setValue("path_tor_inst", path_tor_inst);
    speichern.setValue("path_cur_home", path_cur_home);
    speichern.setValue("size", size);
    speichern.setValue("point", point);
    speichern.setValue("en_max", en_max);
    speichern.setValue("lastUrl", lastUrl);

    speichern.deleteLater();
    delete ui;
}
void settings::but_tor_search_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Pfad zur Tor Installation:"), path_tor_inst, tr("Exe - Dateien (*.exe);;Alle Dateien (*.*)"));
    if(QFile::exists(fileName) == true)
        ui->line_path_to_tor_inst->setText(fileName);
    else
        QMessageBox::information(this, tr("Fehler!"), tr("Datei konnte nicht gefunden werden"), QMessageBox::Ok);
}
void settings::but_info_clicked()
{
    QDialog dialog(this);
    Ui::info_dialog dui;
    dui.setupUi(&dialog);

    dialog.exec();
}
void settings::save()
{
    en_cookies = ui->ch_en_cookies->isChecked();
    en_plugins = ui->ch_en_plugins->isChecked();
    en_tor = ui->gp_en_tor->isChecked();
    en_homePage = ui->ch_atStartup_load_home->isChecked();
    en_sav_set = ui->gp_settings_save->isChecked();
    path_cur_home = ui->line_path_home_seite->text();
    path_tor_inst = ui->line_path_to_tor_inst->text();
    QSettings speichern(QCoreApplication::applicationDirPath() + tr("//settings.ini"), QSettings::IniFormat);
    speichern.setValue("en_sav_set", en_sav_set);
    if(!en_sav_set)
        return;
    speichern.setValue("en_cookies", en_cookies);
    speichern.setValue("en_plugins", en_plugins);
    speichern.setValue("en_tor", en_tor);
    speichern.setValue("en_treeBar", en_treeBar);
    speichern.setValue("path_tor_inst", path_tor_inst);
    speichern.setValue("path_cur_home", path_cur_home);
    speichern.setValue("size", size);
    speichern.setValue("point", point);
    speichern.setValue("en_max", en_max);
    speichern.setValue("lastUrl", lastUrl);

    speichern.deleteLater();
}
void settings::set()
{
    if(!en_plugins)
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);
    else
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    en_cookies = ui->ch_en_cookies->isChecked();
    en_plugins = ui->ch_en_plugins->isChecked();
    en_tor = ui->gp_en_tor->isChecked();
    en_homePage = ui->ch_atStartup_load_home->isChecked();
    en_sav_set = ui->gp_settings_save->isChecked();
    path_cur_home = ui->line_path_home_seite->text();
    path_tor_inst = ui->line_path_to_tor_inst->text();
    emit requestForValues();
    emit settingsChagned();
}
void settings::but_qt_infoclicked()
{
    QApplication::aboutQt();
}
void settings::startRequestForHomePage()
{
    emit requestForCurrentPage();
}
