#ifndef NODEQML_PLUGIN_H
#define NODEQML_PLUGIN_H

#include <QQmlExtensionPlugin>

class NodeQMLPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // NODEQML_PLUGIN_H
