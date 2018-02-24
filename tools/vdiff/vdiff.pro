QT      += core gui widgets svg concurrent sql

TARGET   = vdiff
TEMPLATE = app

CONFIG += C++11

SOURCES  += \
    imageview.cpp \
    main.cpp \
    mainwindow.cpp \
    process.cpp \
    render.cpp \
    settingsdialog.cpp \
    tests.cpp \
    paths.cpp

HEADERS  += \
    imageview.h \
    mainwindow.h \
    process.h \
    render.h \
    settingsdialog.h \
    either.h \
    tests.h \
    paths.h

FORMS    += \
    mainwindow.ui \
    settingsdialog.ui

DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    icons.qrc
