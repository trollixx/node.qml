#include "nodeqml_plugin.h"

#include "node.h"

#include <QQmlEngine>
#include <qqml.h>

static QObject *node_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    return new Node(engine, scriptEngine, engine);
}

void NodeQMLPlugin::registerTypes(const char *uri)
{
    // @uri com.wisetroll.nodeqml
    qmlRegisterSingletonType<Node>(uri, 1, 0, "Node", node_singletontype_provider);
}
