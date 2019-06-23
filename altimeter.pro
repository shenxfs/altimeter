#-------------------------------------------------
#
# Project created by QtCreator 2015-12-30T06:12:54
#
#-------------------------------------------------

QT       += core gui
RC_ICONS += altimeter.ico
RC_FILE = app.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += serialport
TARGET = altimeter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    altimeter.qrc

DISTFILES += \
    app.rc
