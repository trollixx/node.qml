#include "nodeqml_plugin.h"
#include "filesystem.h"

#include <qqml.h>

static QObject *filesystem_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    FileSystem *fs = new FileSystem();
    return fs;
}

void NodeQMLPlugin::registerTypes(const char *uri)
{
    // @uri com.wisetroll.nodeqml
    qmlRegisterSingletonType<FileSystem>(uri, 1, 0, "FileSystem", filesystem_singletontype_provider);
}
