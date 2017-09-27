#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T16:44:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Calculate
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    xycalculatethread.cpp \
    xytreemodel.cpp

HEADERS  += mainwindow.h \
    xycalculatethread.h \
    xytreemodel.h

FORMS    += mainwindow.ui

RC_FILE = ico.rc
