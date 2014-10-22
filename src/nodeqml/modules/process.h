#ifndef PROCESS_H
#define PROCESS_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct ProcessModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

    static QV4::ReturnedValue property_pid_getter(QV4::CallContext *ctx);

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
    /// TODO: process.env
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
    /// TODO: process.nextTick(callback)
    /// TODO: process.maxTickDepth
    /// TODO: process.umask([mask])
    /// TODO: process.uptime()
    /// TODO: process.hrtime()

private:
    static inline QString arch();
    static inline QString platform();
};

} // namespace NodeQml

#endif // PROCESS_H
