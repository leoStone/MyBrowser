#include "bookmarks.h"
#include "ui_addBookmarkDialog.h"
#include "ui_editBookmarksDialog.h"
#include "ui_addBookmarksFolderDialog.h"
#include "ui_editBookmarksFolderDialog.h"

bookmarks::bookmarks(QWidget *parent, QTreeWidget *ptree, QPushButton *pneu,QPushButton *pedit, QPushButton *pnewFolder) :
    QWidget(parent), tree(ptree), neu(pneu), bedit(pedit), newOrdner(pnewFolder)
{
    QFile file(QCoreApplication::applicationDirPath() + tr("//bookmarks.xbel"));
    file.open(QFile::ReadOnly);

    XbelReader reader(tree);
    reader.read(&file);

    connect(neu, SIGNAL(clicked()), this, SLOT(addBookmark()));
    connect(newOrdner, SIGNAL(clicked()), this, SLOT(addFolder()));
    connect(bedit, SIGNAL(clicked()), this, SLOT(edit()));
}
bookmarks::~bookmarks()
{
    QFile file(QCoreApplication::applicationDirPath() + tr("//bookmarks.xbel"));
    file.open(QFile::ReadWrite);
    XbelWriter writer(tree);
    writer.writeFile(&file);
}

void bookmarks::addBookmark()
{
    QDialog dialog(this);
    Ui::addBookmarksDialog ui;
    ui.setupUi(&dialog);

    ui.lineAdress->setText(curAdress);
    ui.lineName->setText(curTitel);

    QList<QTreeWidgetItem *> list = tree->findItems(tr("folder"), Qt::MatchExactly, 0);
    int count = list.count();
    for(int i = 0; i <= count; i++)
    {
        ui.comboFolder->addItem(list.at(i)->text(0));
    }

    if(dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem *item;
        list.at(ui.comboFolder->currentIndex())->addChild(item);
        item->setData(0, Qt::UserRole, tr("bookmark"));
        item->setText(0, ui.lineName->text());
        item->setText(1, ui.lineAdress->text());
    }
}
void bookmarks::addFolder()
{
    QDialog dialog(this);
    Ui::addBookmarksFolderDialog ui;
    ui.setupUi(&dialog);

    ui.lineName->setText(tr("Neuer Ordner"));

    QList<QTreeWidgetItem *> list = tree->findItems(tr("folder"), Qt::MatchExactly, 0);
    int count = list.count();
    for(int i = 0; i <= count; i++)
    {
        ui.comboFolder->addItem(list.at(i)->text(0));
    }
    if(dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem *folder;
        list.at(ui.comboFolder->currentIndex())->addChild(folder);
        folder->setData(0, Qt::UserRole, tr("folder"));
        folder->setText(0, ui.lineName->text());
    }
}
void bookmarks::remove()
{
    tree->takeTopLevelItem(tree->currentIndex().row());
}
void bookmarks::edit()
{
    QTreeWidgetItem *selected = tree->currentItem();
    if(tree->currentItem()->data(0,Qt::UserRole) == "folder")
        editFolder(selected);
    else
        editBookmark(selected);
}
void bookmarks::editFolder(QTreeWidgetItem *selected)
{
    QDialog dialog(this);
    Ui::editBookmarksFolderDialog ui;
    ui.setupUi(&dialog);

    ui.lineName->setText(selected->text(0));

    if(dialog.exec() == QDialog::Accepted)
    {
        selected->setText(0, ui.lineName->text());
    }
}
void bookmarks::editBookmark(QTreeWidgetItem *selected)
{
    QDialog dialog(this);
    Ui::editBookmarksDialog ui;
    ui.setupUi(&dialog);

    ui.lineName->setText(selected->text(0));
    ui.lineAdresse->setText(selected->text(1));

    if(dialog.exec() == QDialog::Accepted)
    {
        selected->setText(0, ui.lineName->text());
        selected->setText(1, ui.lineAdresse->text());
    }
}
