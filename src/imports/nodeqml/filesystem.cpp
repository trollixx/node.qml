#include "filesystem.h"

#include <QFile>
#include <QFileInfo>

FileSystem::FileSystem(QObject *parent):
    QObject(parent)
{

}

FileSystem::~FileSystem()
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
