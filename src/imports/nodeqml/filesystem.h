#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>

class FileSystem : public QObject
{
    Q_OBJECT

public:
    FileSystem(QObject *parent = 0);
    ~FileSystem();
};

#endif // FILESYSTEM_H

