#include "filesystem.h"

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
