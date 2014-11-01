INCLUDEPATH += $$top_srcdir/include
LIBS += -L$$top_builddir/lib -lnodeqml
unix:QMAKE_RPATHDIR += $$top_builddir/lib

DESTDIR += $$top_builddir/bin
