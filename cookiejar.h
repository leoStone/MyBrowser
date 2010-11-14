#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QObject>
#include "includer.h"

class cookiejar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit cookiejar(QObject *parent, bool svaeCookies);
    ~cookiejar();
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
signals:
    void CookieChanged();

public slots:
    void clear();
    void set_m_saveCookies(bool saveCookies);
private slots:
    void save();
private:
    bool m_loaded;
    void load();
    void purgeOldCookies();
    QStringList m_exceptions_block;
    QStringList m_exceptions_allow;
    QStringList m_exceptions_allowForSession;

    bool m_saveCookies;

};

#endif // COOKIEJAR_H
