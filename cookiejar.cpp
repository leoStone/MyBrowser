#include "cookiejar.h"

static const unsigned int JAR_VERSION = 23;

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
    stream << JAR_VERSION;
    stream << quint32(list.size());
    for (int i = 0; i < list.size(); ++i)
        stream << list.at(i).toRawForm();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
    list.clear();

    quint32 version;
    stream >> version;

    if (version != JAR_VERSION)
        return stream;

    quint32 count;
    stream >> count;
    for(quint32 i = 0; i < count; ++i)
    {
        QByteArray value;
        stream >> value;
        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
        if (newCookies.count() == 0 && value.length() != 0) {
            qWarning() << "CookieJar: Unable to parse saved cookie:" << value;
        }
        for (int j = 0; j < newCookies.count(); ++j)
            list.append(newCookies.at(j));
        if (stream.atEnd())
            break;
    }
    return stream;
}
QT_END_NAMESPACE


cookiejar::cookiejar(QObject *parent, bool saveCookies) : QNetworkCookieJar(parent), m_loaded(false), m_saveCookies(saveCookies)
{
}
cookiejar::~cookiejar()
{
    if(!m_saveCookies)
        save();
    else
        clear();
}

void cookiejar::clear()
{
    setAllCookies(QList<QNetworkCookie>());
    emit CookieChanged();
}
void cookiejar::load()
{
    if (m_loaded || !m_saveCookies)
        return;
    // load cookies and exceptions
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
    QSettings cookieSettings(QCoreApplication::applicationDirPath() + tr("\\cookies.ini"), QSettings::IniFormat);
    setAllCookies(qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies"))));
    cookieSettings.deleteLater();

    m_loaded = true;
}
void cookiejar::save()
{
    if (!m_loaded || !m_saveCookies)
        return;
    purgeOldCookies();
    QString directory = QCoreApplication::applicationDirPath();
    QSettings cookieSettings(directory + QLatin1String("/cookies.ini"), QSettings::IniFormat);
    QList<QNetworkCookie> cookies = allCookies();
    for (int i = cookies.count() - 1; i >= 0; --i) {
        if (cookies.at(i).isSessionCookie())
            cookies.removeAt(i);
    }
    cookieSettings.setValue(QLatin1String("cookies"), qVariantFromValue<QList<QNetworkCookie> >(cookies));
}
void cookiejar::purgeOldCookies()
{
    QList<QNetworkCookie> cookies = allCookies();
        if (cookies.isEmpty())
            return;
        int oldCount = cookies.count();
        QDateTime now = QDateTime::currentDateTime();
        for (int i = cookies.count() - 1; i >= 0; --i) {
            if (!cookies.at(i).isSessionCookie() && cookies.at(i).expirationDate() < now)
                cookies.removeAt(i);
        }
        if (oldCount == cookies.count())
            return;
        setAllCookies(cookies);
        emit CookieChanged();
}
QList<QNetworkCookie> cookiejar::cookiesForUrl(const QUrl &url) const
{
    cookiejar *that = const_cast<cookiejar*>(this);
    if (!m_loaded)
        that->load();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled)) {
        QList<QNetworkCookie> noCookies;
        return noCookies;
    }

    return QNetworkCookieJar::cookiesForUrl(url);
}

bool cookiejar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    if (!m_loaded)
        load();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return false;

    QString host = url.host();
    bool eBlock = qBinaryFind(m_exceptions_block.begin(), m_exceptions_block.end(), host) != m_exceptions_block.end();
    bool eAllow = qBinaryFind(m_exceptions_allow.begin(), m_exceptions_allow.end(), host) != m_exceptions_allow.end();
    bool eAllowSession = qBinaryFind(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end(), host) != m_exceptions_allowForSession.end();

    bool addedCookies = false;
    // pass exceptions
    bool acceptInitially = true;
    if ((acceptInitially && !eBlock)
        || (!acceptInitially && (eAllow || eAllowSession))) {
        // pass url domain == cookie domain
        QDateTime soon = QDateTime::currentDateTime();
        soon = soon.addDays(90);
        foreach(QNetworkCookie cookie, cookieList) {
            QList<QNetworkCookie> lst;
            if (!cookie.isSessionCookie()
                && cookie.expirationDate() > soon) {
                    cookie.setExpirationDate(soon);
            }
            lst += cookie;
            if (QNetworkCookieJar::setCookiesFromUrl(lst, url)) {
                addedCookies = true;
            } else {
                // finally force it in if wanted
                    QList<QNetworkCookie> cookies = allCookies();
                    cookies += cookie;
                    setAllCookies(cookies);
                    addedCookies = true;

#if 0
                else
                    qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
#endif
            }
        }
    }

    if (addedCookies) {
        emit CookieChanged();
    }
    return addedCookies;
}
void cookiejar::set_m_saveCookies(bool saveCookies)
{
    m_saveCookies = saveCookies;
}
