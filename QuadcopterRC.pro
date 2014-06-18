#-------------------------------------------------
#
# Project created by QtCreator 2014-06-01T20:53:32
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuadcopterRC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controlarea.cpp \
    grapharea.cpp \
    graphwidget.cpp \
    tracelogstore.cpp

HEADERS  += mainwindow.h \
    controlarea.h \
    grapharea.h \
    graphwidget.h \
    tracelogstore.h

FORMS    += mainwindow.ui \
    controlarea.ui \
    grapharea.ui \
    graphwidget.ui
