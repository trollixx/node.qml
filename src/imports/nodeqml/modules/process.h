#ifndef PROCESS_H
#define PROCESS_H

#include "coremodule.h"

#include <QStringList>

class Process : public CoreModule
{
    Q_OBJECT
    Q_PROPERTY(QStringList argv READ argv CONSTANT)
    Q_PROPERTY(QString execPath READ execPath CONSTANT)
    Q_PROPERTY(int pid READ pid CONSTANT)
    Q_PROPERTY(QString arch READ arch CONSTANT)
    Q_PROPERTY(QString platform READ platform CONSTANT)
public:
    explicit Process(QJSEngine *jsEngine, QObject *parent = nullptr);

    /// TODO: Event: 'exit'
    /// TODO: Event: 'uncaughtException'
    /// TODO: Signal Events
    /// TODO: process.stdout
    /// TODO: process.stderr
    /// TODO: process.stdin
    /// TODO: process.execArgv
    Q_INVOKABLE void abort();
    Q_INVOKABLE bool chdir(const QString &directory);
    Q_INVOKABLE QString cwd() const;
    /// TODO: process.env
    Q_INVOKABLE void exit(int code = 0);
    /// TODO: process.getgid()
    /// TODO: process.setgid(id)
    /// TODO: process.getuid()
    /// TODO: process.setuid(id)
    /// TODO: process.getgroups()
    /// TODO: process.setgroups(groups)
    /// TODO: process.initgroups(user, extra_group)
    /// TODO: process.version
    /// TODO: process.versions
    /// TODO: process.config
    /// TODO: process.kill(pid, [signal])
    /// TODO: process.title
    /// TODO: process.memoryUsage()
    /// TODO: process.nextTick(callback)
    /// TODO: process.maxTickDepth
    /// TODO: process.umask([mask])
    /// TODO: process.uptime()
    /// TODO: process.hrtime()

private:
    QStringList argv() const;
    QString execPath() const;
    int pid() const;
    QString arch() const;
    QString platform() const;

};

#endif // PROCESS_H
