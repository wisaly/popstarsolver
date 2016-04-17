#-------------------------------------------------
#
# Project created by QtCreator 2014-07-24T09:11:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PopStarSolver
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        widget.cpp \
    boardwidget.cpp \
    solution.cpp \
    move.cpp \
    mctsnode.cpp \
    mcts.cpp \
    gameboard.cpp \
    board.cpp

HEADERS  += widget.h \
    boardwidget.h \
    solution.h \
    move.h \
    gameboard.h \
    mcts.h \
    mctsnode.h \
    board.h

FORMS    += widget.ui
