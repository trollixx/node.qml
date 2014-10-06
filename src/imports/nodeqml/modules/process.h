#ifndef PROCESS_H
#define PROCESS_H

#include "coremodule.h"

class Process : public CoreModule
{
    Q_OBJECT
    Q_PROPERTY(int pid READ pid CONSTANT)
public:
    explicit Process(QJSEngine *jsEngine, QObject *parent = nullptr);

    /// TODO: Event: 'exit'
    /// TODO: Event: 'uncaughtException'
    /// TODO: Signal Events
    /// TODO: process.stdout
    /// TODO: process.stderr
    /// TODO: process.stdin
    /// TODO: process.argv
    /// TODO: process.execPath
    /// TODO: process.execArgv
    /// TODO: process.abort()
    /// TODO: process.chdir(directory)
    /// TODO: process.cwd()
    /// TODO: process.env
    /// TODO: process.exit([code])
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
    /// TODO: process.arch
    /// TODO: process.platform
    /// TODO: process.memoryUsage()
    /// TODO: process.nextTick(callback)
    /// TODO: process.maxTickDepth
    /// TODO: process.umask([mask])
    /// TODO: process.uptime()
    /// TODO: process.hrtime()

private:
    int pid() const;

};

#endif // PROCESS_H
