#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "coremodule.h"

class FileSystem : public CoreModule
{
    Q_OBJECT

public:
    explicit FileSystem(QJSEngine *jsEngine, QObject *parent = nullptr);

    // Synchonous API

    Q_INVOKABLE bool existsSync(const QString &path);
    Q_INVOKABLE bool renameSync(const QString)
};

#endif // FILESYSTEM_H

