#-------------------------------------------------
#
# Project created by QtCreator 2014-01-16T15:32:38
#
#-------------------------------------------------

cache()

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tadra
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    floatroutine.cpp \
    graphicobject.cpp \
    graphicwidget.cpp \
    hintwindow.cpp \
    tabdata.cpp \
    headbar.cpp \
    tabcontroller.cpp \
    sheet.cpp \
    book.cpp \
    design.cpp

HEADERS  += window.h \
    floatroutine.h \
    graphicobject.h \
    graphicwidget.h \
    hintwindow.h \
    tabdata.h \
    headbar.h \
    tabcontroller.h \
    singletont.h \
    sheet.h \
    book.h \
    design.h

FORMS += \
    hintwindow.ui

CONFIG += c++11
