#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QWidget>
#include "includer.h"
#include "xbelreader.h"
#include "xbelwriter.h"

namespace ui{
class bookamrks;
}

class bookmarks : public QWidget
{
    Q_OBJECT
public:
    explicit bookmarks(QWidget *parent, QTreeWidget *ptree, QPushButton *pneu,QPushButton *pedit, QPushButton *pnewFolder);
    ~bookmarks();
    QTreeWidget *tree;
    QPushButton *neu;
    QPushButton *bedit;
    QPushButton *newOrdner;

    QString curTitel;
    QString curAdress;

signals:
    void requestForOpenBookmark(QTreeWidgetItem *item);

public slots:
    void addBookmark();
    void addFolder();
    void remove();
    void edit();
    void editBookmark(QTreeWidgetItem *selected);
    void editFolder(QTreeWidgetItem *selected);


};

#endif // BOOKMARKS_H
