QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Logger/logger.cpp \
    commandhandler.cpp \
    localserver.cpp \
    logviewer.cpp \
    main.cpp \
    passworkapi.cpp \
    settingswindow.cpp \
    urlconnectserver.cpp

HEADERS += \
    ../Logger/logger.h \
    commandhandler.h \
    localserver.h \
    logviewer.h \
    passworkapi.h \
    settingswindow.h \
    urlconnectserver.h

FORMS += \
    logviewer.ui \
    settingswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DEFINES += QT_MESSAGELOGCONTEXT
RC_ICONS = TrayIcon.ico
VERSION = 0.1b
QMAKE_TARGET_COMPANY = North Bridge
QMAKE_TARGET_PRODUCT = URLConnect
QMAKE_TARGET_DESCRIPTION = Using a URL to launch programs from a browser
QMAKE_TARGET_COPYRIGHT = North Bridge
