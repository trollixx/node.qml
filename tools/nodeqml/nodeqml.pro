include(../tools.pri)

QT += core qml
QT -= gui

TARGET = nodeqml
CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

LIBS += -lnodeqml
