#include "nodeqml_plugin.h"

#include "dns.h"
#include "filesystem.h"
#include "path.h"
#include "util.h"

#include <QQmlEngine>
#include <qqml.h>

static QObject *dns_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new Dns(engine, engine);
}

static QObject *fs_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new FileSystem(engine);
}

static QObject *path_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new Path(engine);
}

static QObject *util_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new Util(engine);
}

void NodeQMLPlugin::registerTypes(const char *uri)
{
    // @uri com.wisetroll.nodeqml
    qmlRegisterSingletonType<Dns>(uri, 1, 0, "DNS", dns_singletontype_provider);
    qmlRegisterSingletonType<FileSystem>(uri, 1, 0, "FS", fs_singletontype_provider);
    qmlRegisterSingletonType<Path>(uri, 1, 0, "Path", path_singletontype_provider);
    qmlRegisterSingletonType<Util>(uri, 1, 0, "Util", util_singletontype_provider);
}
