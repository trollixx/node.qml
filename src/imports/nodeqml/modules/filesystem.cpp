#include "filesystem.h"

#include <QFile>
#include <QFileInfo>
#include <QJSValue>

FileSystem::FileSystem(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{

}

bool FileSystem::existsSync(const QString &path)
{
    return QFileInfo::exists(path);
}

bool FileSystem::renameSync(const QString &oldPath, const QString &newPath)
{
    return QFile::rename(oldPath, newPath);
}
