#-------------------------------------------------
#
# Project created by QtCreator 2017-11-19T17:07:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SuperPosterize
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    filters.cpp

HEADERS  += mainwindow.h \
    filters.h \
    avir.h

FORMS    += mainwindow.ui
