#include "path.h"

#include <QDir>
#include <QFileInfo>

Path::Path(QObject *parent) :
    QObject(parent)
{
}

QString Path::normalize(const QString &p) const
{
    return QDir::cleanPath(p);
}

QString Path::dirname(const QString &p) const
{
    return QFileInfo(p).absolutePath();
}

QString Path::basename(const QString &p, const QString &ext) const
{
    const QString fileName = QFileInfo(p).fileName();
    if (!ext.isEmpty() && p.endsWith(ext))
        return fileName.left(fileName.size() - ext.size());
    else
        return fileName;
}

QString Path::extname(const QString &p) const
{
    return QFileInfo(p).completeSuffix();
}

QString Path::sep() const
{
    return QDir::separator();
}

QString Path::delimiter() const
{
#ifdef Q_OS_WIN
    return QStringLiteral(";");
#else
    return QStringLiteral(":");
#endif
}
