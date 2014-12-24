CONFIG += testcase parallel_test c++11
QT += qml testlib

TARGET = tst_node
SOURCES += tst_node.cpp

INCLUDEPATH += $$top_srcdir/src
LIBS += -L$$top_builddir/lib -lnodeqml
unix:QMAKE_RPATHDIR += $$top_builddir/lib
