TEMPLATE = lib
TARGET = NodeQML
QT += core-private qml qml-private quick network
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.wisetroll.nodeqml

# Input
SOURCES += \
    nodeqml_plugin.cpp \
    coremodule.cpp \
    engine.cpp \
    globalextensions.cpp \
    moduleobject.cpp \
    modules/dns.cpp \
    modules/filesystem.cpp \
    modules/os.cpp \
    modules/path.cpp \
    modules/process.cpp \
    modules/util.cpp \
    types/buffer.cpp

HEADERS += \
    nodeqml_plugin.h \
    coremodule.h \
    engine.h \
    globalextensions.h \
    moduleobject.h \
    modules/dns.h \
    modules/filesystem.h \
    modules/os.h \
    modules/path.h \
    modules/process.h \
    modules/util.h \
    types/buffer.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
