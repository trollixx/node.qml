#include "nodeqml_plugin.h"

#include "engine.h"

#include <QQmlEngine>
#include <qqml.h>

void NodeQMLPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    new NodeQml::Engine(engine, engine);
}

void NodeQMLPlugin::registerTypes(const char *uri)
{
    // @uri com.wisetroll.nodeqml
    /// TODO: Get rid of this stupid registration
    qmlRegisterType<QObject>(uri, 1, 0, "__nodeQmlUselessObject");
}
