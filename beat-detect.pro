#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T12:58:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = beat-detect
TEMPLATE = app

CONFIG += C++11

include(rdh_bd/rdh_bd.pri)


SOURCES += main.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h


