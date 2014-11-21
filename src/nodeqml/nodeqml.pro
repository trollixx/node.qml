QT += core-private qml qml-private quick network
CONFIG += c++11

TARGET = $$qtLibraryTarget(nodeqml)
TEMPLATE = lib

DEFINES += NODEQML_LIBRARY

SOURCES += \
    engine.cpp \
    globalextensions.cpp \
    moduleobject.cpp \
    modules/console.cpp \
    modules/dns.cpp \
    modules/filesystem.cpp \
    modules/os.cpp \
    modules/path.cpp \
    modules/process.cpp \
    modules/util.cpp \
    types/buffer.cpp \
    types/errnoexception.cpp

HEADERS_PUBLIC += \
    nodeqml_global.h \
    engine.h

HEADERS_PRIVATE += \
    engine_p.h \
    globalextensions.h \
    v4integration.h \
    moduleobject.h \
    modules/console.h \
    modules/dns.h \
    modules/filesystem.h \
    modules/os.h \
    modules/path.h \
    modules/process.h \
    modules/util.h \
    types/buffer.h \
    types/errnoexception.h

HEADERS += $$HEADERS_PUBLIC $$HEADERS_PRIVATE

RESOURCES += \
    js.qrc

DESTDIR = $$top_builddir/lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}
