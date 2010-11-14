#-------------------------------------------------
#
# Project created by QtCreator 2010-09-13T12:44:24
#
#-------------------------------------------------
TEMPLATE = app

TARGET = MyBrowser

QT       += core gui webkit network xml

FORMS    += \
    mainwindow.ui \
    authentification.ui \
    settings.ui \
    info_dialog.ui \
    addBookmarkDialog.ui \
    addBookmarksFolderDialog.ui \
    editBookmarksFolderDialog.ui \
    editBookmarksDialog.ui \
    downloadmanager.ui

HEADERS  += mainwindow.h \
    gsuggests.h \
    update.h \
    favicon.h \
    includer.h \
    cookiejar.h \
    settings.h \
    bookmarks.h \
    xbelwriter.h \
    xbelreader.h \
    downloadmanager.h

SOURCES += main.cpp\
        mainwindow.cpp \
    gsuggests.cpp \
    update.cpp \
    favicon.cpp \
    cookiejar.cpp \
    settings.cpp \
    bookmarks.cpp \
    xbelwriter.cpp \
    xbelreader.cpp \
    downloadmanager.cpp

RESOURCES += \
    res.qrc

win32 {RC_FILE = "icon.rc"}

OTHER_FILES +=
