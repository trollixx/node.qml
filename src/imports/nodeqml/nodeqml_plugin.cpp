#include "nodeqml_plugin.h"

#include "filesystem.h"
#include "util.h"

#include <QQmlEngine>
#include <qqml.h>

static QObject *fs_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new FileSystem(engine);
}

static QObject *util_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new Util(engine);
}

void NodeQMLPlugin::registerTypes(const char *uri)
{
    // @uri com.wisetroll.nodeqml
    qmlRegisterSingletonType<FileSystem>(uri, 1, 0, "FS", fs_singletontype_provider);
    qmlRegisterSingletonType<Util>(uri, 1, 0, "Util", util_singletontype_provider);
}
