#-------------------------------------------------
#
# Project created by QtCreator 2015-12-30T06:12:54
#
#-------------------------------------------------

QT       += core gui
RC_ICONS += Altimeter.ico
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
