#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>

class FileSystem : public QObject
{
    Q_OBJECT

public:
    FileSystem(QObject *parent = 0);
    ~FileSystem();

    // Synchonous API

    Q_INVOKABLE bool existsSync(const QString &path);
    Q_INVOKABLE bool renameSync(const QString)
};

#endif // FILESYSTEM_H

