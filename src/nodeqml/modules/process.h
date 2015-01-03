#ifndef PROCESS_H
#define PROCESS_H

#include "../v4integration.h"

#include <private/qv4object_p.h>

namespace NodeQml {

namespace Heap {

struct ProcessModule : QV4::Heap::Object {
    ProcessModule(QV4::ExecutionEngine *v4);
};

} // namespace Heap

struct ProcessModule : QV4::Object
{
    NODE_V4_OBJECT(ProcessModule, Object)

    static QV4::ReturnedValue property_pid_getter(QV4::CallContext *ctx);
    static QV4::ReturnedValue property_env_getter(QV4::CallContext *ctx);

    /// TODO: Event: 'exit'
    /// TODO: Event: 'uncaughtException'
    /// TODO: Signal Events
    /// TODO: process.stdout
    /// TODO: process.stderr
    /// TODO: process.stdin
    /// TODO: process.execArgv
    static QV4::ReturnedValue method_abort(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_chdir(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_cwd(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_exit(QV4::CallContext *ctx);
    /// TODO: process.getgid()
    /// TODO: process.setgid(id)
    /// TODO: process.getuid()
    /// TODO: process.setuid(id)
    /// TODO: process.getgroups()
    /// TODO: process.setgroups(groups)
    /// TODO: process.initgroups(user, extra_group)
    /// TODO: process.versions
    /// TODO: process.config
    /// TODO: process.kill(pid, [signal])
    /// TODO: process.title
    /// TODO: process.memoryUsage()
    static QV4::ReturnedValue method_nextTick(QV4::CallContext *ctx);
    /// TODO: process.maxTickDepth
    /// TODO: process.umask([mask])
    /// TODO: process.uptime()
    /// TODO: process.hrtime()

    static QString arch();
    static QString platform();
};

} // namespace NodeQml

#endif // PROCESS_H
