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
    Patcher.cpp \
    Filer.cpp \
    DropArea.cpp

HEADERS  += Widget.h \
    Patcher.h \
    Filer.h \
    ColorLog.h \
    DropArea.h

FORMS    += Widget.ui

RESOURCES += \
    zUnlock-ZN5.qrc

TRANSLATIONS += \
    l10n/zUnlock-ZN5_ru.ts \
    l10n/zUnlock-ZN5_en.ts

OTHER_FILES += patch.map \
    zUnlock-ZN5.rc \
    ReadMe.md

# Windows Executable Icon
win32:RC_FILE = zUnlock-ZN5.rc
