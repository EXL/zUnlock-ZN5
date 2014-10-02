#-------------------------------------------------
#
# Project created by QtCreator 2014-10-02T00:52:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zUnlock-ZN5
TEMPLATE = app


SOURCES += main.cpp\
        Widget.cpp \
    Patcher.cpp

HEADERS  += Widget.h \
    Patcher.h

FORMS    += Widget.ui

RESOURCES += \
    zUnlock-ZN5.qrc

OTHER_FILES = patch.map
