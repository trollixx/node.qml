#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>

class FileSystem : public QObject
{
    Q_OBJECT

public:
    FileSystem(QObject *parent = 0);
    ~FileSystem();

    Q_INVOKABLE bool existsSync(const QString &path);
};

#endif // FILESYSTEM_H

