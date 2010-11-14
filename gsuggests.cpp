#include "gsuggests.h"
#define GSUGGEST_URL "http://google.com/complete/search?output=toolbar&q=%1"

gsuggests::gsuggests(QLineEdit *parent) :
    QObject(parent), editor(parent)
{
    QFont font;
    font.setPointSize(10);
    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(2);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setHeaderHidden(true);

    popup->setFont(font);

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(doneCompletition()));

    timer = new QTimer(this);

    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), SLOT(autoSuggest()));
    connect(editor, SIGNAL(textEdited(QString)), timer, SLOT(start()));

    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
                 this, SLOT(handleNetworkData(QNetworkReply*)));

}
gsuggests::~gsuggests()
{
    delete popup;
}
bool gsuggests::eventFilter(QObject *obj, QEvent *ev)
 {
    if (obj != popup)
        return false;

     if (ev->type() == QEvent::MouseButtonPress) {
         popup->hide();
         editor->setFocus();
         return true;
     }

     if (ev->type() == QEvent::KeyPress) {

         bool consumed = false;
         int key = static_cast<QKeyEvent*>(ev)->KeyPress;
         switch (key) {
         case Qt::Key_Enter:
         case Qt::Key_Return:
             doneCompletition();
             consumed = true;

         case Qt::Key_Escape:
             editor->setFocus();
             popup->hide();
             consumed = true;

         case Qt::Key_Up:
         case Qt::Key_Down:
         case Qt::Key_Home:
         case Qt::Key_End:
         case Qt::Key_PageUp:
         case Qt::Key_PageDown:
             break;

         default:
             editor->setFocus();
             editor->event(ev);
             break;
         }

         return consumed;
     }

     return false;
 }
void gsuggests::showCompletion(const QStringList &choices, const QStringList &hits)
 {

     if (choices.isEmpty() || choices.count() != hits.count())
         return;

     const QPalette &pal = editor->palette();
     QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

     popup->setUpdatesEnabled(false);
     popup->clear();
     for (int i = 0; i < choices.count(); ++i) {
         QTreeWidgetItem * item;
         item = new QTreeWidgetItem(popup);
         item->setText(0, choices[i]);
         item->setText(1, hits[i]);
         item->setTextAlignment(1, Qt::AlignRight);
         item->setTextColor(1, color);
     }
     popup->setCurrentItem(popup->topLevelItem(0));
     popup->resizeColumnToContents(0);
     popup->resizeColumnToContents(1);
     popup->adjustSize();
     popup->setUpdatesEnabled(true);

     int h = popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
     popup->resize(editor->width(), h);

     popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
     popup->setFocus();
     popup->show();
 }
void gsuggests::doneCompletition()
 {
     timer->stop();
     popup->hide();
     editor->setFocus();
     QTreeWidgetItem *item = popup->currentItem();
     if (item) {
         QString eingabe = item->text(0);
         eingabe.insert(0,"google.de/search?hl=de&source=hp&q=");
         editor->setText(eingabe);
         QMetaObject::invokeMethod(editor, "returnPressed");
     }
 }
void gsuggests::autoSuggest()
{
    QString str = editor->text();
    QString url = QString(GSUGGEST_URL).arg(str);
    networkManager.get(QNetworkRequest(QString(url)));
}
void gsuggests::preventSuggest()
{
    timer->stop();
}
void gsuggests::handleNetworkData(QNetworkReply *networkReply)
 {
     QUrl url = networkReply->url();
     if (!networkReply->error()) {
         QStringList choices;
         QStringList hits;

         QByteArray response(networkReply->readAll());
         QXmlStreamReader xml(response);
         while (!xml.atEnd()) {
             xml.readNext();
             if (xml.tokenType() == QXmlStreamReader::StartElement)
                 if (xml.name() == "suggestion") {
                     QStringRef str = xml.attributes().value("data");
                     choices << str.toString();
                 }
             if (xml.tokenType() == QXmlStreamReader::StartElement)
                 if (xml.name() == "num_queries") {
                     QStringRef str = xml.attributes().value("int");
                     hits << str.toString();
                 }
         }

         showCompletion(choices, hits);
     }

     networkReply->deleteLater();
 }
