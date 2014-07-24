#-------------------------------------------------
#
# Project created by QtCreator 2014-07-24T09:11:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PopStarSolver
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    board.cpp \
    boardwidget.cpp

HEADERS  += widget.h \
    board.h \
    boardwidget.h

FORMS    += widget.ui
