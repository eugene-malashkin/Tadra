#-------------------------------------------------
#
# Project created by QtCreator 2014-01-16T15:32:38
#
#-------------------------------------------------

cache()

QT       += core gui network xml

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
    design.cpp \
    dlgtablabel.cpp \
    graphicbutton.cpp \
    base.cpp \
    documentbody.cpp \
    documentboxbuttons.cpp \
    gridcoordinategenerator.cpp \
    gridscale.cpp \
    placeroutine.cpp \
    documentlayer.cpp \
    documentbox.cpp \
    chartroutine.cpp \
    colorroutine.cpp \
    currencychartwidget.cpp \
    currencyinstrument.cpp \
    numeral.cpp \
    searchengine.cpp \
    searchinput.cpp \
    searchinputhighlight.cpp \
    timelyaction.cpp

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
    design.h \
    dlgtablabel.h \
    graphicbutton.h \
    base.h \
    documentbody.h \
    documentboxbuttons.h \
    gridcoordinategenerator.h \
    gridscale.h \
    placeroutine.h \
    indexsortheplert.h \
    documentlayer.h \
    documentbox.h \
    chartroutine.h \
    colorroutine.h \
    currencychartwidget.h \
    currencyinstrument.h \
    numeral.h \
    searchengine.h \
    searchinput.h \
    searchinputhighlight.h \
    timelyaction.h

FORMS += \
    hintwindow.ui \
    dlgtablabel.ui

CONFIG += c++11

RESOURCES += \
    resources.qrc
